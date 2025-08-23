# Makefile for Footballer Network Simulation
# Enhanced with OpenCV support for image classification

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -g

# NS-3 configuration
NS3_CFLAGS = $(shell pkg-config --cflags ns3-core ns3-network ns3-internet ns3-point-to-point ns3-applications ns3-netanim ns3-mobility ns3-flow-monitor)
NS3_LIBS = $(shell pkg-config --libs ns3-core ns3-network ns3-internet ns3-point-to-point ns3-applications ns3-netanim ns3-mobility ns3-flow-monitor)

# OpenCV configuration
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Combined flags
ALL_CFLAGS = $(CXXFLAGS) $(NS3_CFLAGS) $(OPENCV_CFLAGS)
ALL_LIBS = $(NS3_LIBS) $(OPENCV_LIBS)

# Source files
SOURCES = footballer-network-sim.cc image_classifier.cpp
OBJECTS = $(SOURCES:.cc=.o) $(SOURCES:.cpp=.o)
TARGET = footballer-network-sim

# Default target
all: $(TARGET)

# Main target
$(TARGET): footballer-network-sim.o image_classifier.o
	@echo "Linking $(TARGET)..."
	$(CXX) -o $@ $^ $(ALL_LIBS)
	@echo "Build completed successfully!"

# Compile C++ source files
%.o: %.cc
	@echo "Compiling $<..."
	$(CXX) $(ALL_CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(ALL_CFLAGS) -c $< -o $@

# Setup directories and download required files
setup: 
	@echo "Setting up project directories and files..."
	@chmod +x setup_images.sh
	@./setup_images.sh

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f *.o $(TARGET)
	rm -f *.xml *.tr *.pcap
	@echo "Clean completed."

# Deep clean including results
distclean: clean
	@echo "Deep cleaning all generated files..."
	rm -rf results/
	rm -f *.yml *.json
	@echo "Deep clean completed."

# Run simulation with default parameters
run: $(TARGET)
	@echo "Running simulation with default parameters..."
	./$(TARGET)

# Run simulation with real images (if available)
run-real: $(TARGET)
	@echo "Running simulation with real image classification..."
	./$(TARGET) --useRealImages=true --trainDir=./training_images --imageDir=./images

# Run simulation and generate analysis
analyze: run
	@echo "Analyzing results..."
	python3 analyze_results.py
	python3 image_analysis.py

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y libopencv-dev libopencv-contrib-dev
	sudo apt-get install -y python3-matplotlib python3-seaborn python3-pandas
	@echo "Dependencies installed."

# Check system requirements
check:
	@echo "Checking system requirements..."
	@pkg-config --exists opencv4 && echo "✓ OpenCV4 found" || echo "✗ OpenCV4 not found"
	@pkg-config --exists ns3-core && echo "✓ NS-3 found" || echo "✗ NS-3 not found"
	@python3 -c "import matplotlib, seaborn, pandas, numpy" 2>/dev/null && echo "✓ Python packages found" || echo "✗ Python packages missing"
	@echo "Requirements check completed."

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the simulation (default)"
	@echo "  setup        - Setup directories and download required files"
	@echo "  clean        - Remove build files"
	@echo "  distclean    - Remove all generated files"
	@echo "  run          - Run simulation with simulated images"
	@echo "  run-real     - Run simulation with real image classification"
	@echo "  analyze      - Run simulation and analyze results"
	@echo "  install-deps - Install system dependencies"
	@echo "  check        - Check system requirements"
	@echo "  help         - Show this help"

# Prevent make from treating these as file targets
.PHONY: all clean distclean run run-real analyze setup install-deps check help
# This provides convenient commands for building and running the simulation

.PHONY: help build run clean scenarios analyze install-deps

# Default target
help:
	@echo "Football Network Simulation - Available Commands:"
	@echo ""
	@echo "  make build          - Build the NS-3 simulation"
	@echo "  make run            - Run the basic simulation"
	@echo "  make scenarios      - Run all predefined scenarios"
	@echo "  make analyze        - Analyze the last simulation results"
	@echo "  make clean          - Clean build artifacts and results"
	@echo "  make install-deps   - Install Python dependencies"
	@echo "  make netanim        - Open NetAnim visualization"
	@echo ""
	@echo "Advanced usage:"
	@echo "  make run ARGS='--simulationTime=20 --dataRate=10Mbps'"
	@echo "  make scenario NAME=scenario_high_bandwidth"
	@echo ""

# Build the simulation
build:
	@echo "Building NS-3 simulation..."
	cd .. && ./ns3 configure --enable-examples --enable-tests
	cd .. && ./ns3 build

# Run basic simulation with default parameters
run: build
	@echo "Running basic simulation..."
	cd .. && ./ns3 run "CN-ns3-project/footballer-network-sim $(ARGS)"

# Run a specific scenario
scenario: build
	@if [ -z "$(NAME)" ]; then \
		echo "Error: Please specify scenario name with NAME=scenario_name"; \
		echo "Available scenarios:"; \
		python3 run_scenarios.py --list; \
		exit 1; \
	else \
		python3 run_scenarios.py --scenario $(NAME); \
	fi

# Run all scenarios
scenarios: build
	@echo "Running all simulation scenarios..."
	python3 run_scenarios.py

# Analyze results
analyze:
	@if [ -f "flowmon-results.xml" ]; then \
		python3 analyze_results.py; \
	else \
		echo "No simulation results found. Run 'make run' first."; \
	fi

# List available scenarios
list-scenarios:
	@python3 run_scenarios.py --list

# Install Python dependencies
install-deps:
	@echo "Installing Python dependencies..."
	pip3 install matplotlib pandas numpy configparser

# Open NetAnim if available
netanim:
	@if [ -f "footballer-network-anim.xml" ]; then \
		if command -v netanim >/dev/null 2>&1; then \
			netanim footballer-network-anim.xml; \
		else \
			echo "NetAnim not found. Please install NetAnim to view visualizations."; \
			echo "Animation file available: footballer-network-anim.xml"; \
		fi; \
	else \
		echo "No NetAnim file found. Run simulation with NetAnim enabled first."; \
	fi

# Clean build artifacts and results
clean:
	@echo "Cleaning build artifacts and results..."
	rm -f footballer-network-anim.xml
	rm -f flowmon-results.xml
	rm -f *.log
	rm -rf results/
	rm -rf scenario_results/
	cd .. && ./ns3 clean

# Run simulation with custom parameters
custom-run: build
	@echo "Running simulation with custom parameters..."
	@echo "Usage: make custom-run TIME=15 SIZE=100000 RATE=5Mbps DELAY=5ms"
	cd .. && ./ns3 run "CN-ns3-project/footballer-network-sim \
		--simulationTime=$(or $(TIME),10) \
		--imageSize=$(or $(SIZE),50000) \
		--dataRate=$(or $(RATE),1Mbps) \
		--delay=$(or $(DELAY),2ms) \
		--packetSize=$(or $(PACKET),1024)"

# Quick test run
test: build
	@echo "Running quick test simulation..."
	cd .. && ./ns3 run "CN-ns3-project/footballer-network-sim \
		--simulationTime=5 \
		--imageSize=10000 \
		--enableNetAnim=false"

# Performance test with different configurations
perf-test: build
	@echo "Running performance tests..."
	@echo "Test 1: Low bandwidth"
	cd .. && ./ns3 run "CN-ns3-project/footballer-network-sim \
		--simulationTime=15 \
		--dataRate=256Kbps \
		--delay=50ms \
		--enableNetAnim=false"
	@echo "Test 2: High bandwidth"
	cd .. && ./ns3 run "CN-ns3-project/footballer-network-sim \
		--simulationTime=10 \
		--dataRate=100Mbps \
		--delay=1ms \
		--enableNetAnim=false"

# Check system requirements
check-deps:
	@echo "Checking system dependencies..."
	@echo -n "NS-3: "
	@if [ -f "../ns3" ]; then echo "✓ Found"; else echo "✗ Not found"; fi
	@echo -n "Python3: "
	@if command -v python3 >/dev/null 2>&1; then echo "✓ Found"; else echo "✗ Not found"; fi
	@echo -n "Make: "
	@if command -v make >/dev/null 2>&1; then echo "✓ Found"; else echo "✗ Not found"; fi
	@echo -n "NetAnim: "
	@if command -v netanim >/dev/null 2>&1; then echo "✓ Found"; else echo "✗ Not found (optional)"; fi

# Show project status
status:
	@echo "Project Status:"
	@echo "  Configuration: $(if $(wildcard simulation_config.ini),✓ Found,✗ Missing)"
	@echo "  Main simulation: $(if $(wildcard footballer-network-sim.cc),✓ Found,✗ Missing)"
	@echo "  Analysis script: $(if $(wildcard analyze_results.py),✓ Found,✗ Missing)"
	@echo "  Recent results: $(if $(wildcard flowmon-results.xml),✓ Found,✗ None)"
	@echo "  NetAnim file: $(if $(wildcard footballer-network-anim.xml),✓ Found,✗ None)"

# Development helpers
dev-setup: install-deps
	@echo "Setting up development environment..."
	chmod +x run_simulation.sh
	chmod +x analyze_results.py
	chmod +x run_scenarios.py

# Backup results
backup:
	@echo "Creating backup of results..."
	@timestamp=$$(date +%Y%m%d_%H%M%S); \
	backup_dir="backup_$$timestamp"; \
	mkdir -p "$$backup_dir"; \
	cp -r results/ "$$backup_dir/" 2>/dev/null || true; \
	cp -r scenario_results/ "$$backup_dir/" 2>/dev/null || true; \
	cp *.xml "$$backup_dir/" 2>/dev/null || true; \
	echo "Backup created: $$backup_dir"
