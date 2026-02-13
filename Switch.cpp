#include "Switch.h"
#include <iostream>
#include <iomanip>

// ANSI color codes for better output readability
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

Switch::Switch(int ports, int timeout) 
    : numPorts(ports), agingTimeout(timeout), currentCycle(0),
      framesProcessed(0), learningEvents(0), forwardingEvents(0), floodingEvents(0) {
    std::cout << CYAN << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║  Layer 2 Ethernet Learning Switch Simulator   ║\n";
    std::cout << "╚════════════════════════════════════════════════╝" << RESET << "\n";
    std::cout << "Switch initialized with " << numPorts << " ports\n";
    if (agingTimeout > 0) {
        std::cout << "MAC aging enabled: " << agingTimeout << " seconds\n";
    }
    std::cout << std::string(50, '-') << "\n\n";
}

void Switch::processFrame(const Frame& frame, int incomingPort) {
    processFrame(frame.sourceMAC, frame.destMAC, incomingPort);
}

void Switch::processFrame(const std::string& sourceMAC, const std::string& destMAC, int incomingPort) {
    framesProcessed++;
    
    std::cout << BLUE << "Frame #" << framesProcessed << " received on Port " << incomingPort << RESET << "\n";
    std::cout << "  Source MAC: " << GREEN << sourceMAC << RESET << "\n";
    std::cout << "  Dest MAC:   " << YELLOW << destMAC << RESET << "\n";
    
    // Step 1: LEARNING PHASE
    // Associate the source MAC with the incoming port
    auto it = macTable.find(sourceMAC);
    if (it == macTable.end()) {
        // New MAC address - add to table
        macTable.emplace(sourceMAC, MACTableEntry(incomingPort));
        learningEvents++;
        std::cout << "  " << GREEN << "✓ LEARNING:" << RESET 
                  << " Added " << sourceMAC << " -> Port " << incomingPort << "\n";
    } else if (it->second.port != incomingPort) {
        // MAC moved to a different port - update
        std::cout << "  " << YELLOW << "⚠ UPDATE:" << RESET 
                  << " " << sourceMAC << " moved from Port " 
                  << it->second.port << " to Port " << incomingPort << "\n";
        it->second.port = incomingPort;
        it->second.timestamp = std::chrono::steady_clock::now();
    } else {
        // MAC seen again on same port - refresh timestamp
        it->second.timestamp = std::chrono::steady_clock::now();
        std::cout << "  " << "↻ REFRESH:" << " " << sourceMAC 
                  << " timestamp updated on Port " << incomingPort << "\n";
    }
    
    // Step 2: FORWARDING DECISION
    std::cout << "  ";
    
    // Check for broadcast address
    if (destMAC == "FF:FF:FF:FF:FF:FF") {
        std::cout << MAGENTA << "⚡ BROADCAST:" << RESET 
                  << " Flooding to all ports except Port " << incomingPort << "\n";
        floodingEvents++;
        
        // Show which ports would receive the frame
        std::cout << "    Flooding ports: ";
        for (int i = 1; i <= numPorts; i++) {
            if (i != incomingPort) {
                std::cout << i << " ";
            }
        }
        std::cout << "\n";
    } else {
        // Unicast destination - check MAC table
        auto destIt = macTable.find(destMAC);
        if (destIt != macTable.end()) {
            // KNOWN UNICAST - forward to specific port
            int outPort = destIt->second.port;
            if (outPort == incomingPort) {
                // Destination is on the same segment - filter/drop
                std::cout << CYAN << "⊗ FILTERING:" << RESET 
                          << " Destination on same port (Port " << incomingPort << ") - frame dropped\n";
            } else {
                std::cout << GREEN << "→ FORWARDING:" << RESET 
                          << " Sending to Port " << outPort << " (Known Unicast)\n";
                forwardingEvents++;
            }
        } else {
            // UNKNOWN UNICAST - flood all ports except incoming
            std::cout << RED << "⚠ UNKNOWN UNICAST:" << RESET 
                      << " Destination " << destMAC << " not in MAC table\n";
            std::cout << "    Flooding to all ports except Port " << incomingPort << "\n";
            floodingEvents++;
            
            // Show which ports would receive the frame
            std::cout << "    Flooding ports: ";
            for (int i = 1; i <= numPorts; i++) {
                if (i != incomingPort) {
                    std::cout << i << " ";
                }
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\n";
}

void Switch::cleanupTable() {
    if (agingTimeout <= 0) {
        return; // Aging disabled
    }
    
    auto now = std::chrono::steady_clock::now();
    int removed = 0;
    
    for (auto it = macTable.begin(); it != macTable.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - it->second.timestamp).count();
        
        if (elapsed > agingTimeout) {
            std::cout << YELLOW << "⌛ AGING OUT: " << RESET 
                      << it->first << " (last seen " << elapsed << "s ago)\n";
            it = macTable.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    
    if (removed > 0) {
        std::cout << "Removed " << removed << " aged entries from MAC table\n\n";
    }
}

void Switch::printMACTable() const {
    std::cout << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║           Current MAC Address Table            ║\n";
    std::cout << "╚════════════════════════════════════════════════╝" << RESET << "\n";
    
    if (macTable.empty()) {
        std::cout << "  (Empty - no MAC addresses learned yet)\n";
        return;
    }
    
    std::cout << std::left << std::setw(20) << "MAC Address" 
              << std::setw(10) << "Port" 
              << "Age (seconds)\n";
    std::cout << std::string(50, '-') << "\n";
    
    auto now = std::chrono::steady_clock::now();
    for (const auto& entry : macTable) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            now - entry.second.timestamp).count();
        
        std::cout << std::left << std::setw(20) << entry.first
                  << std::setw(10) << entry.second.port
                  << age << "s\n";
    }
    std::cout << "\n";
}

void Switch::printStatistics() const {
    std::cout << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║              Switch Statistics                 ║\n";
    std::cout << "╚════════════════════════════════════════════════╝" << RESET << "\n";
    std::cout << "Total Frames Processed:  " << framesProcessed << "\n";
    std::cout << "Learning Events:         " << learningEvents << "\n";
    std::cout << "Forwarding Events:       " << forwardingEvents << "\n";
    std::cout << "Flooding Events:         " << floodingEvents << "\n";
    std::cout << "MAC Table Size:          " << macTable.size() << " entries\n";
    
    if (framesProcessed > 0) {
        double forwardingRate = (100.0 * forwardingEvents) / framesProcessed;
        double floodingRate = (100.0 * floodingEvents) / framesProcessed;
        std::cout << "Forwarding Efficiency:   " << std::fixed << std::setprecision(1) 
                  << forwardingRate << "% (higher is better)\n";
        std::cout << "Flooding Rate:           " << floodingRate << "%\n";
    }
    std::cout << "\n";
}

void Switch::clearMACTable() {
    macTable.clear();
    std::cout << "MAC table cleared\n\n";
}

void Switch::advanceCycle() {
    currentCycle++;
}

bool Switch::isLearned(const std::string& mac) const {
    return macTable.find(mac) != macTable.end();
}
