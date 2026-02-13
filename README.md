# L2-Sim: Ethernet Learning Switch Simulator

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

A professional-grade simulation of an **Ethernet Learning Switch** operating at **Layer 2 (Data Link Layer)** of the OSI model. This project demonstrates fundamental networking concepts including MAC address learning, frame forwarding logic, and switch intelligence.

## 🎯 Project Overview

This simulator replicates the behavior of a hardware network switch, showing how it makes forwarding decisions based on a dynamically learned MAC address table. Perfect for understanding:

- **OSI Layer 2 (Data Link Layer)** operation
- MAC address table maintenance
- Frame forwarding vs. flooding decisions
- Network switch intelligence

## 🧠 How It Works

### Core Switch Logic

The simulator implements the standard learning switch algorithm:

```
1. LEARNING PHASE
   - When a frame arrives, record the source MAC address
   - Associate it with the incoming port number
   - Update/refresh timestamp for aging

2. FORWARDING DECISION
   - Check if destination MAC is in the table
   - If KNOWN → Forward to specific port (Known Unicast)
   - If UNKNOWN → Flood to all ports except incoming (Unknown Unicast)
   - If BROADCAST (FF:FF:FF:FF:FF:FF) → Flood to all ports
```

### MAC Address Table

The switch maintains a **MAC Address Table** (also called a CAM table or forwarding table) using `std::unordered_map<std::string, MACTableEntry>`:

```cpp
struct MACTableEntry {
    int port;                    // Physical port number
    timestamp;                   // For aging/timeout
}
```

### Frame Processing Types

| Traffic Type | Destination | Action | Efficiency |
|--------------|-------------|--------|------------|
| **Known Unicast** | MAC in table | Forward to specific port | ⚡ High |
| **Unknown Unicast** | MAC not in table | Flood all ports (except incoming) | ⚠️ Low |
| **Broadcast** | FF:FF:FF:FF:FF:FF | Flood all ports (except incoming) | ⚠️ Low |

## 📂 Project Structure

```
L2-Sim/
├── main.cpp           # Entry point with simulation scenarios
├── Switch.h           # Switch class declaration
├── Switch.cpp         # Core switching logic implementation
├── Frame.h            # Ethernet frame data structure
├── Makefile           # Build automation
└── README.md          # This file
```

## 🚀 Quick Start

### Prerequisites

- C++17 compatible compiler (g++, clang++)
- Make utility

### Build & Run

```bash
# Clone the repository
git clone https://github.com/yourusername/L2-Sim.git
cd L2-Sim

# Compile
make

# Run the simulation
./l2sim
```

### Makefile Commands

```bash
make         # Build the project
make run     # Build and run
make clean   # Remove build artifacts
make rebuild # Clean and rebuild
make help    # Show available commands
```

## 📊 Example Output

### Phase 1: Initial Discovery (Unknown Unicast)

When devices first communicate, the switch must learn their locations:

```
Frame #1 received on Port 1
  Source MAC: AA:AA:AA:AA:AA:AA
  Dest MAC:   FF:FF:FF:FF:FF:FF
  ✓ LEARNING: Added AA:AA:AA:AA:AA:AA -> Port 1
  ⚡ BROADCAST: Flooding to all ports except Port 1
    Flooding ports: 2 3 4 5 6 7 8

Frame #2 received on Port 2
  Source MAC: BB:BB:BB:BB:BB:BB
  Dest MAC:   AA:AA:AA:AA:AA:AA
  ✓ LEARNING: Added BB:BB:BB:BB:BB:BB -> Port 2
  → FORWARDING: Sending to Port 1 (Known Unicast)
```

### Phase 2: Efficient Forwarding (Known Unicast)

Once MACs are learned, the switch forwards intelligently:

```
Frame #5 received on Port 1
  Source MAC: AA:AA:AA:AA:AA:AA
  Dest MAC:   BB:BB:BB:BB:BB:BB
  ↻ REFRESH: AA:AA:AA:AA:AA:AA timestamp updated on Port 1
  → FORWARDING: Sending to Port 2 (Known Unicast)
```

### Current MAC Address Table

```
╔════════════════════════════════════════════════╗
║           Current MAC Address Table           ║
╚════════════════════════════════════════════════╝
MAC Address         Port      Age (seconds)
--------------------------------------------------
DD:DD:DD:DD:DD:DD   4         0s
CC:CC:CC:CC:CC:CC   3         0s
BB:BB:BB:BB:BB:BB   2         2s
AA:AA:AA:AA:AA:AA   1         3s
```

### Switch Statistics

```
╔════════════════════════════════════════════════╗
║              Switch Statistics                 ║
╚════════════════════════════════════════════════╝
Total Frames Processed:  12
Learning Events:         4
Forwarding Events:       8
Flooding Events:         4
MAC Table Size:          4 entries
Forwarding Efficiency:   66.7% (higher is better)
Flooding Rate:           33.3%
```

## 💡 Key Features

### ✅ Core Features
- **MAC Address Learning**: Automatic discovery of device locations
- **Known Unicast Forwarding**: Efficient frame delivery to learned destinations
- **Unknown Unicast Flooding**: Handles frames to unknown destinations
- **Broadcast Handling**: Proper flooding of broadcast frames
- **Port Filtering**: Prevents frames from returning to source port

### 🎁 Bonus Features
- **MAC Table Aging**: Removes stale entries after timeout (configurable)
- **Device Mobility**: Detects and updates MAC addresses that move between ports
- **Real-time Statistics**: Tracks forwarding efficiency and flooding rate
- **Colorized Output**: Enhanced terminal visualization
- **Multiple Scenarios**: Comprehensive test cases demonstrating different behaviors

## 🔧 Technical Implementation

### Data Structures

```cpp
// MAC Address Table Entry
struct MACTableEntry {
    int port;                                           // Port number
    std::chrono::steady_clock::time_point timestamp;    // For aging
};

// MAC Address Table
std::unordered_map<std::string, MACTableEntry> macTable;

// Ethernet Frame
struct Frame {
    std::string sourceMAC;      // Source MAC address
    std::string destMAC;        // Destination MAC address
    std::string etherType;      // Protocol type
    std::string payload;        // Frame data
};
```

### Algorithm Complexity

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Frame Processing | O(1) average | O(n) for n MACs |
| MAC Lookup | O(1) average | - |
| Table Aging | O(n) | - |

### OSI Model Context

```
Application Layer    (Layer 7)  ← User applications
Presentation Layer   (Layer 6)  ← Data formatting
Session Layer        (Layer 5)  ← Connection management
Transport Layer      (Layer 4)  ← TCP/UDP
Network Layer        (Layer 3)  ← IP routing
**Data Link Layer**  (Layer 2)  ← **This Project** (Ethernet switching)
Physical Layer       (Layer 1)  ← Physical transmission
```

## 📚 Concepts Demonstrated

### Networking Fundamentals
- **Layer 2 Switching**: Understanding the Data Link Layer
- **MAC Addresses**: 48-bit hardware addresses
- **Forwarding Tables**: Dynamic learning and lookup
- **Broadcast Domains**: All ports in the same broadcast domain

### Real-World Scenarios
1. **Network Startup**: Devices discover each other via flooding
2. **Steady State**: Efficient forwarding after learning
3. **New Device Joins**: Dynamic table updates
4. **Device Moves**: MAC mobility handling
5. **Aging**: Automatic cleanup of stale entries

## 🎓 Learning Objectives

This project demonstrates understanding of:

- ✅ OSI Model Layer 2 operation
- ✅ MAC address table management
- ✅ Forwarding vs. flooding decision logic
- ✅ Network efficiency optimization
- ✅ C++ STL containers (`unordered_map`, `chrono`)
- ✅ Object-oriented design patterns
- ✅ Professional code documentation

## 🛠️ Customization

### Adjust Switch Configuration

```cpp
// In main.cpp, modify constructor parameters:
Switch mySwitch(numPorts, agingTimeout);

// Examples:
Switch mySwitch(24, 300);    // 24 ports, 5-minute aging
Switch mySwitch(8, 0);        // 8 ports, no aging
```

### Add Custom Scenarios

```cpp
// Create your own network topology
std::string MAC_SERVER = "AA:BB:CC:DD:EE:FF";
std::string MAC_CLIENT = "11:22:33:44:55:66";

// Simulate traffic
mySwitch.processFrame(MAC_CLIENT, MAC_SERVER, 1);
mySwitch.processFrame(MAC_SERVER, MAC_CLIENT, 2);
```

## 🚀 Future Enhancements

Potential improvements for extended learning:

- [ ] VLAN support (multiple broadcast domains)
- [ ] Spanning Tree Protocol (STP) simulation
- [ ] Port mirroring/SPAN capability
- [ ] MAC address table size limits
- [ ] Statistics export (CSV, JSON)
- [ ] Interactive CLI mode
- [ ] Pcap file input/output
- [ ] GUI visualization with network diagram

## 📝 License

MIT License - feel free to use this project for learning and portfolio purposes.

## 🤝 Contributing

This is an educational project, but improvements are welcome:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📧 Contact

Perfect for networking/systems engineering internship portfolios!

For questions or feedback, please open an issue on GitHub.

---

**Keywords**: C++, Networking, Layer 2, Ethernet, Switch, MAC Address, OSI Model, Data Link Layer, Forwarding, Computer Networks, Systems Programming
