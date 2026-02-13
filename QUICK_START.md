# Quick Start Guide

## 🚀 Get Running in 60 Seconds

### 1. Clone & Build
```bash
git clone https://github.com/yourusername/L2-Sim.git
cd L2-Sim
make
```

### 2. Run
```bash
./l2sim
```

That's it! You'll see a colorful simulation demonstrating switch learning.

## 📊 What You'll See

### Output Highlights

**Green**: New MAC learned  
**Yellow**: Broadcast or unknown destination  
**Cyan**: Headers and tables  
**Red**: Flooding events  

### Key Phases

1. **Phase 1: Initial Discovery** - Watch unknown unicast flooding
2. **Phase 2: Efficient Forwarding** - See known unicast forwarding
3. **Phase 3: New Device** - Dynamic learning demonstration
4. **Phase 4: Broadcasts** - Broadcast handling
5. **Aging Demo** - MAC timeout simulation
6. **Mobility Demo** - Device movement handling

## 🎓 Understanding the Output

### Frame Processing

```
Frame #1 received on Port 1
  Source MAC: AA:AA:AA:AA:AA:AA
  Dest MAC:   BB:BB:BB:BB:BB:BB
  ✓ LEARNING: Added AA:AA:AA:AA:AA:AA -> Port 1
  → FORWARDING: Sending to Port 2 (Known Unicast)
```

**Interpretation**:
- Frame arrives from device with MAC AA:AA:AA:AA:AA:AA on Port 1
- Switch learns this association
- Destination BB:BB:BB:BB:BB:BB is already known on Port 2
- Frame is forwarded directly (efficient!)

### MAC Table

```
MAC Address         Port      Age (seconds)
--------------------------------------------------
AA:AA:AA:AA:AA:AA   1         0s
BB:BB:BB:BB:BB:BB   2         0s
CC:CC:CC:CC:CC:CC   3         2s
```

**What This Means**:
- Three devices currently known
- Device AA is on Port 1, seen just now (0s ago)
- Device CC is on Port 3, last seen 2 seconds ago

### Statistics

```
Forwarding Efficiency:   66.7% (higher is better)
Flooding Rate:           33.3%
```

**Interpretation**:
- 66.7% of frames were forwarded directly to specific ports
- 33.3% required flooding (broadcasts or unknown destinations)
- Higher forwarding efficiency = better learning

## 🔧 Customization

### Modify Scenarios

Edit `main.cpp` to create your own network:

```cpp
void myCustomScenario() {
    Switch mySwitch(16, 600);  // 16 ports, 10-min aging
    
    // Your custom traffic here
    mySwitch.processFrame("AA:AA:AA:AA:AA:AA", "BB:BB:BB:BB:BB:BB", 1);
}
```

### Adjust Simulation Speed

Comment out the aging demo in `main.cpp` for faster execution:

```cpp
// demonstrateAging();  // Skip 6-second delay
```

## 📚 Next Steps

- Read `README.md` for comprehensive overview
- Check `DESIGN.md` for technical deep dive
- Experiment with custom scenarios
- Star the repo if you found it helpful!

## 💡 Interview Talking Points

When discussing this project:

1. **Layer 2 Knowledge**: "I implemented a learning switch that operates at the Data Link Layer"
2. **Algorithms**: "Uses O(1) hash map lookups to mimic hardware CAM tables"
3. **Real Behavior**: "Handles unknown unicast flooding, broadcast domains, and MAC aging"
4. **Optimization**: "Tracks forwarding efficiency to measure learning effectiveness"
5. **Scalability**: "Designed with enterprise switch behavior in mind"

## 🐛 Troubleshooting

### Compilation Issues

```bash
# If you see "c++17" errors:
# Edit Makefile and change to c++11 or c++14

# If you see "make: command not found":
g++ -std=c++17 -o l2sim main.cpp Switch.cpp
```

### Runtime Issues

```bash
# If colors don't display:
# Your terminal may not support ANSI codes
# Output will still be readable, just not colorized
```

## 🎯 Recommended Use Cases

- **Networking course projects**
- **Technical interview prep**
- **Portfolio demonstrations**
- **Teaching tool for Layer 2 concepts**
- **Understanding switch behavior before using real equipment**

## 🌟 Make It Your Own

Ideas to extend this project:

1. Add command-line arguments for configuration
2. Implement VLAN support
3. Create a GUI with network visualization
4. Add pcap file input/output
5. Implement Spanning Tree Protocol (STP)
6. Add port security features
7. Create REST API for remote control

---

**Happy Switching!** 🌐
