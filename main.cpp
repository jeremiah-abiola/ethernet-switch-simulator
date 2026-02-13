#include <iostream>
#include <thread>
#include <chrono>
#include "Switch.h"
#include "Frame.h"

// ANSI color codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

/**
 * @brief Simulates a realistic network scenario with multiple devices
 * 
 * Topology:
 *   PC-A (Port 1) - MAC: AA:AA:AA:AA:AA:AA
 *   PC-B (Port 2) - MAC: BB:BB:BB:BB:BB:BB
 *   PC-C (Port 3) - MAC: CC:CC:CC:CC:CC:CC
 *   PC-D (Port 4) - MAC: DD:DD:DD:DD:DD:DD
 */
void runSimulation() {
    std::cout << BOLD << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║         SIMULATION: Network Startup            ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n" << RESET;
    std::cout << "Simulating a fresh network where devices communicate\n";
    std::cout << "for the first time. Watch how the switch learns!\n\n";
    
    // Create a switch with 8 ports, 300 second aging
    Switch mySwitch(8, 300);
    
    // Define MAC addresses for our virtual PCs
    std::string MAC_PC_A = "AA:AA:AA:AA:AA:AA";
    std::string MAC_PC_B = "BB:BB:BB:BB:BB:BB";
    std::string MAC_PC_C = "CC:CC:CC:CC:CC:CC";
    std::string MAC_PC_D = "DD:DD:DD:DD:DD:DD";
    std::string MAC_BROADCAST = "FF:FF:FF:FF:FF:FF";
    
    // ===== PHASE 1: INITIAL LEARNING =====
    std::cout << MAGENTA << "═══════════════════════════════════════════════\n";
    std::cout << "PHASE 1: Initial Discovery (Unknown Unicast)\n";
    std::cout << "═══════════════════════════════════════════════" << RESET << "\n\n";
    
    // PC-A sends an ARP request to find PC-B (broadcast)
    std::cout << "Scenario: PC-A broadcasts ARP request 'Who has PC-B?'\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_A, MAC_BROADCAST, 1);
    
    // PC-B responds to PC-A (but switch doesn't know PC-A's location yet from switch's perspective)
    std::cout << "Scenario: PC-B responds to PC-A's ARP\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_B, MAC_PC_A, 2);
    
    // PC-A tries to ping PC-C (unknown unicast - will flood)
    std::cout << "Scenario: PC-A pings PC-C (destination unknown)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_A, MAC_PC_C, 1);
    
    // PC-C responds to PC-A
    std::cout << "Scenario: PC-C responds to PC-A's ping\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_C, MAC_PC_A, 3);
    
    mySwitch.printMACTable();
    
    // ===== PHASE 2: EFFICIENT FORWARDING =====
    std::cout << MAGENTA << "═══════════════════════════════════════════════\n";
    std::cout << "PHASE 2: Known Unicast Forwarding\n";
    std::cout << "═══════════════════════════════════════════════" << RESET << "\n\n";
    std::cout << "Now all MACs are learned. Watch the efficient forwarding!\n\n";
    
    // PC-A to PC-B (known unicast)
    std::cout << "Scenario: PC-A sends data to PC-B (known destination)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_A, MAC_PC_B, 1);
    
    // PC-B to PC-C (known unicast)
    std::cout << "Scenario: PC-B sends data to PC-C (known destination)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_B, MAC_PC_C, 2);
    
    // PC-C to PC-A (known unicast)
    std::cout << "Scenario: PC-C sends data to PC-A (known destination)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_C, MAC_PC_A, 3);
    
    // ===== PHASE 3: NEW DEVICE JOINS =====
    std::cout << MAGENTA << "═══════════════════════════════════════════════\n";
    std::cout << "PHASE 3: New Device Joins Network\n";
    std::cout << "═══════════════════════════════════════════════" << RESET << "\n\n";
    
    // PC-D joins and broadcasts
    std::cout << "Scenario: PC-D (new device) broadcasts DHCP discovery\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_D, MAC_BROADCAST, 4);
    
    // PC-A tries to reach the new PC-D (unknown unicast)
    std::cout << "Scenario: PC-A tries to reach new PC-D (unknown destination)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_A, MAC_PC_D, 1);
    
    // PC-D responds to PC-A
    std::cout << "Scenario: PC-D responds to PC-A\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_D, MAC_PC_A, 4);
    
    // Now PC-A to PC-D is known unicast
    std::cout << "Scenario: PC-A sends more data to PC-D (now known)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_A, MAC_PC_D, 1);
    
    // ===== PHASE 4: BROADCAST TRAFFIC =====
    std::cout << MAGENTA << "═══════════════════════════════════════════════\n";
    std::cout << "PHASE 4: Broadcast Traffic Handling\n";
    std::cout << "═══════════════════════════════════════════════" << RESET << "\n\n";
    
    std::cout << "Scenario: PC-B broadcasts network announcement\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_PC_B, MAC_BROADCAST, 2);
    
    // Display final state
    mySwitch.printMACTable();
    mySwitch.printStatistics();
}

/**
 * @brief Demonstrates MAC address aging functionality
 */
void demonstrateAging() {
    std::cout << BOLD << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║      DEMONSTRATION: MAC Table Aging            ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n" << RESET;
    std::cout << "This demo shows how switches remove old MAC entries\n";
    std::cout << "to handle moved devices and free up table space.\n\n";
    
    // Create switch with 5-second aging for demo purposes
    Switch mySwitch(4, 5);
    
    std::string MAC_A = "AA:AA:AA:AA:AA:AA";
    std::string MAC_B = "BB:BB:BB:BB:BB:BB";
    
    // Learn two MACs
    std::cout << "Learning two MAC addresses...\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_A, MAC_B, 1);
    mySwitch.processFrame(MAC_B, MAC_A, 2);
    
    mySwitch.printMACTable();
    
    std::cout << "Waiting 6 seconds for aging...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(6));
    
    // Try to cleanup aged entries
    std::cout << "Running cleanup...\n";
    mySwitch.cleanupTable();
    
    mySwitch.printMACTable();
    
    // Show that new traffic re-learns the MAC
    std::cout << "New frame arrives - MAC re-learned:\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_A, MAC_B, 1);
    
    mySwitch.printMACTable();
}

/**
 * @brief Shows what happens when a device moves to a different port
 */
void demonstrateMACMove() {
    std::cout << BOLD << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║       DEMONSTRATION: Device Mobility           ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n" << RESET;
    std::cout << "Simulating a laptop that unplugs from Port 1 and\n";
    std::cout << "reconnects to Port 3 (e.g., moved to different room)\n\n";
    
    Switch mySwitch(4, 0); // No aging for this demo
    
    std::string MAC_LAPTOP = "AA:BB:CC:DD:EE:FF";
    std::string MAC_SERVER = "11:22:33:44:55:66";
    
    // Initial position
    std::cout << "Laptop connects to Port 1\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_LAPTOP, MAC_SERVER, 1);
    mySwitch.processFrame(MAC_SERVER, MAC_LAPTOP, 2);
    
    mySwitch.printMACTable();
    
    // Laptop moves
    std::cout << MAGENTA << "⚡ Laptop physically moved to Port 3 ⚡" << RESET << "\n\n";
    
    std::cout << "Laptop sends frame from new location (Port 3)\n";
    std::cout << std::string(50, '-') << "\n";
    mySwitch.processFrame(MAC_LAPTOP, MAC_SERVER, 3);
    
    mySwitch.printMACTable();
    mySwitch.printStatistics();
}

int main() {
    std::cout << "\n";
    std::cout << BOLD << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                      ║\n";
    std::cout << "║        L2-Sim: Ethernet Learning Switch Simulator    ║\n";
    std::cout << "║                                                      ║\n";
    std::cout << "║              OSI Layer 2 (Data Link Layer)           ║\n";
    std::cout << "║                                                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n" << RESET;
    
    // Run main simulation
    runSimulation();
    
    // Demonstrate aging (optional - can be commented out for faster execution)
    std::cout << "\n" << std::string(60, '=') << "\n";
    demonstrateAging();
    
    // Demonstrate MAC mobility
    std::cout << "\n" << std::string(60, '=') << "\n";
    demonstrateMACMove();
    
    std::cout << BOLD << CYAN << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║           Simulation Complete!                 ║\n";
    std::cout << "╚════════════════════════════════════════════════╝" << RESET << "\n\n";
    
    std::cout << "Key Concepts Demonstrated:\n";
    std::cout << "  ✓ MAC Address Learning\n";
    std::cout << "  ✓ Known Unicast Forwarding\n";
    std::cout << "  ✓ Unknown Unicast Flooding\n";
    std::cout << "  ✓ Broadcast Handling\n";
    std::cout << "  ✓ MAC Table Aging\n";
    std::cout << "  ✓ Device Mobility\n\n";
    
    return 0;
}
