# Makefile for L2-Sim Ethernet Learning Switch Simulator

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = l2sim
SOURCES = main.cpp Switch.cpp
HEADERS = Frame.h Switch.h
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Rebuild from scratch
rebuild: clean all

# Help
help:
	@echo "L2-Sim Makefile Commands:"
	@echo "  make         - Build the simulator"
	@echo "  make run     - Build and run the simulator"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make rebuild - Clean and rebuild"
	@echo "  make help    - Show this help message"

.PHONY: all run clean rebuild help
