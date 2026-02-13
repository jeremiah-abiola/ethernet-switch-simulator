#ifndef SWITCH_H
#define SWITCH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "Frame.h"

/**
 * @brief Simulates a Layer 2 Ethernet Learning Switch
 * 
 * This class implements the core logic of a hardware switch operating at the
 * Data Link Layer (OSI Layer 2). It maintains a MAC address table for efficient
 * frame forwarding and demonstrates:
 * - MAC Address Learning
 * - Known Unicast Forwarding
 * - Unknown Unicast Flooding
 * - Broadcast Handling
 * - MAC Table Aging (optional)
 */
class Switch {
private:
    /**
     * @brief Entry in the MAC address table
     */
    struct MACTableEntry {
        int port;                                           // Port number where MAC was learned
        std::chrono::steady_clock::time_point timestamp;    // Last seen time (for aging)
        
        MACTableEntry(int p) 
            : port(p), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    // MAC Address Table: Maps MAC addresses to port numbers and timestamps
    std::unordered_map<std::string, MACTableEntry> macTable;
    
    // Total number of ports on the switch
    int numPorts;
    
    // Aging timeout in seconds (for MAC table cleanup)
    int agingTimeout;
    
    // Simulation cycle counter (alternative to real time)
    int currentCycle;
    
    // Statistics
    int framesProcessed;
    int learningEvents;
    int forwardingEvents;
    int floodingEvents;
    
public:
    /**
     * @brief Constructs a new Switch object
     * 
     * @param ports Number of physical ports on the switch
     * @param timeout MAC address aging timeout in seconds (0 = no aging)
     */
    Switch(int ports = 8, int timeout = 300);
    
    /**
     * @brief Processes an incoming Ethernet frame
     * 
     * This is the core switching logic:
     * 1. Learning: Associates source MAC with incoming port
     * 2. Forwarding decision:
     *    - Known Unicast: Forward to specific port
     *    - Unknown Unicast/Broadcast: Flood all ports except incoming
     * 
     * @param frame The Ethernet frame to process
     * @param incomingPort The port number where the frame arrived
     */
    void processFrame(const Frame& frame, int incomingPort);
    
    /**
     * @brief Overloaded version with individual MAC parameters
     * 
     * @param sourceMAC Source MAC address
     * @param destMAC Destination MAC address
     * @param incomingPort Port where frame arrived
     */
    void processFrame(const std::string& sourceMAC, const std::string& destMAC, int incomingPort);
    
    /**
     * @brief Removes aged-out entries from the MAC table
     * 
     * Real switches implement aging to handle:
     * - Moved devices (same MAC appears on different port)
     * - Disconnected devices (free up table space)
     * - Topology changes
     */
    void cleanupTable();
    
    /**
     * @brief Displays the current MAC address table
     */
    void printMACTable() const;
    
    /**
     * @brief Displays switch statistics
     */
    void printStatistics() const;
    
    /**
     * @brief Clears all learned MAC addresses
     */
    void clearMACTable();
    
    /**
     * @brief Advances the simulation cycle (for aging)
     */
    void advanceCycle();
    
    /**
     * @brief Gets the number of entries in the MAC table
     */
    int getMACTableSize() const { return macTable.size(); }
    
    /**
     * @brief Checks if a MAC address is in the table
     */
    bool isLearned(const std::string& mac) const;
};

#endif // SWITCH_H
