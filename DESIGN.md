# Design Document: L2-Sim Architecture

## Overview

This document provides a technical deep dive into the L2-Sim Ethernet Learning Switch Simulator, explaining design decisions, architectural patterns, and implementation details.

## System Architecture

### High-Level Design

```
┌─────────────────────────────────────────────────┐
│              Main Simulation Driver              │
│  (Creates scenarios, manages test cases)         │
└──────────────────┬──────────────────────────────┘
                   │
                   │ processFrame()
                   ↓
┌─────────────────────────────────────────────────┐
│              Switch Class                        │
│  ┌───────────────────────────────────────────┐  │
│  │      MAC Address Table (Hash Map)         │  │
│  │   Key: MAC Address (string)               │  │
│  │   Value: { Port, Timestamp }              │  │
│  └───────────────────────────────────────────┘  │
│                                                  │
│  Core Methods:                                   │
│  • processFrame()  - Main switching logic        │
│  • cleanupTable()  - Aging mechanism             │
│  • printMACTable() - Diagnostics                 │
└─────────────────────────────────────────────────┘
                   │
                   │ Forward/Flood decision
                   ↓
┌─────────────────────────────────────────────────┐
│          Virtual Network Ports (1-N)             │
│  [Port 1] [Port 2] [Port 3] ... [Port N]        │
└─────────────────────────────────────────────────┘
```

## Core Components

### 1. Frame Structure

**File**: `Frame.h`

```cpp
struct Frame {
    std::string sourceMAC;      // 48-bit MAC (e.g., "AA:BB:CC:DD:EE:FF")
    std::string destMAC;        // 48-bit MAC
    std::string etherType;      // Protocol identifier
    std::string payload;        // Frame data
};
```

**Design Rationale**:
- Uses strings for MAC addresses to prioritize readability over performance
- In production hardware, MACs would be stored as `uint8_t[6]` or `uint64_t`
- Struct over class - no encapsulation needed for simple data container

### 2. Switch Class

**File**: `Switch.h`, `Switch.cpp`

#### MAC Address Table

```cpp
struct MACTableEntry {
    int port;
    std::chrono::steady_clock::time_point timestamp;
};

std::unordered_map<std::string, MACTableEntry> macTable;
```

**Design Decisions**:

| Choice | Rationale |
|--------|-----------|
| `unordered_map` over `map` | O(1) average lookup vs O(log n) - matches hardware CAM table behavior |
| String keys | Human-readable, easy debugging. In hardware: TCAM (Ternary Content Addressable Memory) |
| Timestamp | Enables aging mechanism, reflects real switch behavior |

#### Processing Algorithm

```
Algorithm: Process_Ethernet_Frame
Input: sourceMAC, destMAC, incomingPort
Output: Forwarding decision

1. LEARNING PHASE:
   if sourceMAC NOT in macTable:
       macTable[sourceMAC] = {incomingPort, currentTime}
       print "LEARNING: Added sourceMAC"
   else if macTable[sourceMAC].port ≠ incomingPort:
       macTable[sourceMAC].port = incomingPort
       macTable[sourceMAC].timestamp = currentTime
       print "UPDATE: MAC moved"
   else:
       macTable[sourceMAC].timestamp = currentTime
       print "REFRESH: Timestamp updated"

2. FORWARDING PHASE:
   if destMAC == "FF:FF:FF:FF:FF:FF":
       action = BROADCAST
       floodPorts = ALL_PORTS - {incomingPort}
   else if destMAC in macTable:
       outPort = macTable[destMAC].port
       if outPort == incomingPort:
           action = FILTER
       else:
           action = FORWARD
   else:
       action = UNKNOWN_UNICAST
       floodPorts = ALL_PORTS - {incomingPort}

3. EXECUTE:
   Execute action (FORWARD, BROADCAST, FILTER, UNKNOWN_UNICAST)
   Update statistics
```

## Key Algorithms

### 1. MAC Learning

**Complexity**: O(1) average case

```cpp
void Switch::processFrame(...) {
    // Learning logic
    auto it = macTable.find(sourceMAC);
    if (it == macTable.end()) {
        // New MAC - O(1) insert
        macTable.emplace(sourceMAC, MACTableEntry(incomingPort));
    } else {
        // Update existing - O(1) access
        it->second.port = incomingPort;
        it->second.timestamp = now();
    }
}
```

### 2. Forwarding Decision

**Complexity**: O(1) average case

```cpp
// Check if destination is known
auto destIt = macTable.find(destMAC);  // O(1) average
if (destIt != macTable.end()) {
    // Known unicast - forward to specific port
    int outPort = destIt->second.port;
    forward(frame, outPort);
} else {
    // Unknown unicast - flood
    flood(frame, incomingPort);
}
```

### 3. MAC Table Aging

**Complexity**: O(n) where n = number of entries

```cpp
void Switch::cleanupTable() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = macTable.begin(); it != macTable.end(); ) {
        auto elapsed = duration_cast<seconds>(now - it->second.timestamp).count();
        if (elapsed > agingTimeout) {
            it = macTable.erase(it);  // O(1) per element
        } else {
            ++it;
        }
    }
}
```

**Optimization Note**: Real switches use separate aging threads or timers rather than linear scans.

## Traffic Flow Patterns

### Pattern 1: Cold Start (Unknown Unicast)

```
Time  Event                          MAC Table State              Action
---   ------                         ----------------             ------
t0    Frame A→B arrives Port 1       {A:1}                        Flood
t1    Frame B→A arrives Port 2       {A:1, B:2}                   Forward to 1
t2    Frame A→B arrives Port 1       {A:1, B:2}                   Forward to 2
```

### Pattern 2: Broadcast Traffic

```
Time  Event                          MAC Table State              Action
---   ------                         ----------------             ------
t0    ARP A→FF:FF arrives Port 1     {A:1}                        Flood all
t1    ARP Reply B→A arrives Port 2   {A:1, B:2}                   Forward to 1
```

### Pattern 3: Device Mobility

```
Time  Event                          MAC Table State              Action
---   ------                         ----------------             ------
t0    Frame A→B arrives Port 1       {A:1}                        Flood
t1    [Device A moves to Port 3]     {A:1}                        -
t2    Frame A→B arrives Port 3       {A:3} [updated]              Forward
```

## Performance Characteristics

### Time Complexity

| Operation | Average Case | Worst Case | Hardware Equivalent |
|-----------|--------------|------------|---------------------|
| Frame Processing | O(1) | O(n) | CAM lookup (constant time) |
| MAC Learning | O(1) | O(n) | CAM write (constant time) |
| Forwarding Lookup | O(1) | O(n) | CAM lookup (constant time) |
| Aging Cleanup | O(n) | O(n) | Background process |

### Space Complexity

| Component | Space | Notes |
|-----------|-------|-------|
| MAC Table | O(n) | n = number of unique MACs |
| Frame Processing | O(1) | No additional space per frame |
| Statistics | O(1) | Fixed counters |

### Scalability

```
MACs Learned | Expected Behavior
-------------|-------------------
0-100        | Excellent (typical small network)
100-1000     | Good (typical office)
1000-10000   | Fair (large network, may need table size limits)
10000+       | Poor (need TCAM-like optimization)
```

**Real Hardware**: Modern switches support 8K-32K MAC addresses using specialized hardware (TCAM).

## Comparison: Simulation vs. Real Hardware

| Aspect | L2-Sim | Real Ethernet Switch |
|--------|--------|----------------------|
| **MAC Lookup** | Hash table (O(1) avg) | TCAM (O(1) guaranteed) |
| **Learning** | Software learning | Hardware learning (ASIC) |
| **Forwarding** | Simulated output | Physical wire-speed forwarding |
| **Aging** | Periodic cleanup | Hardware timers per entry |
| **Table Size** | Memory limited | TCAM limited (8K-32K entries) |
| **Speed** | Software (μs-ms) | Hardware (nanoseconds) |
| **Cost** | Free | $100-$10000+ |

## Memory Layout

### MAC Table Memory Model

```
┌────────────────────────────────────────────────┐
│  std::unordered_map<string, MACTableEntry>     │
├────────────────────────────────────────────────┤
│  Bucket Array (dynamic size)                   │
│  ┌──────────────────────────────────────────┐  │
│  │ Bucket 0: → Node → Node → nullptr        │  │
│  │ Bucket 1: → Node → nullptr               │  │
│  │ Bucket 2: → nullptr                      │  │
│  │ ...                                       │  │
│  └──────────────────────────────────────────┘  │
│                                                 │
│  Each Node contains:                            │
│  ┌────────────────────────────────────────┐    │
│  │ Key: std::string (MAC address)         │    │
│  │ Value: { int port, timestamp }         │    │
│  │ Next: Node*                             │    │
│  └────────────────────────────────────────┘    │
└────────────────────────────────────────────────┘

Approximate memory per entry: 
- Key (string): ~32 bytes (SSO + heap if long)
- Value (MACTableEntry): 16 bytes (8B port + 8B timestamp)
- Hash table overhead: ~8-16 bytes
Total: ~56-64 bytes per MAC entry
```

## Real-World Switch Behavior

### Features Not Implemented (But Worth Knowing)

1. **VLAN Support**: Multiple broadcast domains on same physical switch
2. **Spanning Tree Protocol (STP)**: Loop prevention
3. **Port Security**: MAC address limiting per port
4. **Port Mirroring**: Copy traffic for monitoring
5. **QoS/Priority Queuing**: Traffic prioritization
6. **Jumbo Frames**: Support for >1500 byte frames
7. **Link Aggregation**: Bonding multiple ports

### Features Implemented

✅ MAC learning  
✅ Known unicast forwarding  
✅ Unknown unicast flooding  
✅ Broadcast handling  
✅ MAC aging  
✅ Device mobility detection  
✅ Port filtering (same-port drops)  

## Testing Strategy

### Test Cases Covered

1. **Initial Discovery**: Cold start, all MACs unknown
2. **Steady State**: All MACs learned, efficient forwarding
3. **New Device**: Dynamic learning of new MAC
4. **Broadcast**: Proper flooding behavior
5. **Aging**: Timeout and cleanup
6. **Mobility**: MAC moves between ports

### Validation Approach

```
For each test scenario:
1. Set up initial state
2. Send frame
3. Verify:
   - Correct MAC table update
   - Correct forwarding decision
   - Correct statistics
4. Print results for manual inspection
```

## Design Patterns Used

### 1. **Encapsulation**
- Switch class hides MAC table implementation
- Public interface: `processFrame()`, `printMACTable()`
- Private data: `macTable`, statistics

### 2. **Data Structure Abstraction**
- `Frame` struct abstracts Ethernet frame
- `MACTableEntry` encapsulates port and aging info

### 3. **Separation of Concerns**
- `Frame.h`: Data structures
- `Switch.h/cpp`: Business logic
- `main.cpp`: Test scenarios and UI

## Future Optimization Opportunities

### Performance

1. **Pre-allocated hash table**: Reserve space for expected MAC count
```cpp
macTable.reserve(1024);  // Avoid rehashing
```

2. **Aging optimization**: Use circular buffer for time-ordered entries
```cpp
std::deque<pair<string, timestamp>> agingQueue;
```

3. **MAC address optimization**: Use binary format instead of strings
```cpp
struct MACAddress {
    uint8_t bytes[6];  // 6 bytes vs ~32 bytes for string
};
```

### Features

1. **Multithreading**: Parallel frame processing
2. **Statistics export**: CSV/JSON output
3. **Configuration file**: Load topology from file
4. **Interactive mode**: CLI for real-time testing

## Conclusion

This design prioritizes:
- **Clarity**: Readable code over micro-optimizations
- **Education**: Demonstrates networking concepts clearly
- **Realism**: Mirrors actual switch behavior where practical
- **Extensibility**: Easy to add new features

The simulator successfully demonstrates OSI Layer 2 switching concepts while maintaining professional code quality suitable for technical portfolios.
