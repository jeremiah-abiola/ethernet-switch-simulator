#ifndef FRAME_H
#define FRAME_H

#include <string>

/**
 * @brief Represents an Ethernet Frame (OSI Layer 2)
 * 
 * This struct models a simplified Ethernet frame containing:
 * - Source MAC address (48-bit, represented as string)
 * - Destination MAC address (48-bit, represented as string)
 * - EtherType field (e.g., 0x0800 for IPv4, 0x0806 for ARP)
 * - Payload data
 */
struct Frame {
    std::string sourceMAC;      // Source MAC address (e.g., "AA:BB:CC:DD:EE:FF")
    std::string destMAC;        // Destination MAC address
    std::string etherType;      // Protocol type (e.g., "IPv4", "ARP")
    std::string payload;        // Frame payload/data
    
    /**
     * @brief Constructs a new Frame object
     * 
     * @param src Source MAC address
     * @param dest Destination MAC address
     * @param type EtherType/Protocol
     * @param data Payload data
     */
    Frame(const std::string& src, const std::string& dest, 
          const std::string& type = "IPv4", const std::string& data = "")
        : sourceMAC(src), destMAC(dest), etherType(type), payload(data) {}
};

#endif // FRAME_H
