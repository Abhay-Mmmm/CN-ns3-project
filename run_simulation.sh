#!/bin/bash

# Football Network Simulation Runner Script
# This script helps run the NS-3 simulation with different parameters

echo "=== Football Network Simulation Runner ==="
echo ""

# Default parameters
SIMULATION_TIME=10
IMAGE_SIZE=50000
PACKET_SIZE=1024
DATA_RATE="1Mbps"
DELAY="2ms"
ENABLE_NETANIM=true

# Function to display usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -t, --time TIME          Simulation time in seconds (default: $SIMULATION_TIME)"
    echo "  -s, --image-size SIZE    Simulated image size in bytes (default: $IMAGE_SIZE)"
    echo "  -p, --packet-size SIZE   Packet size in bytes (default: $PACKET_SIZE)"
    echo "  -r, --data-rate RATE     Data rate (default: $DATA_RATE)"
    echo "  -d, --delay DELAY        Link delay (default: $DELAY)"
    echo "  -n, --no-netanim         Disable NetAnim output"
    echo "  -h, --help               Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Run with default parameters"
    echo "  $0 -t 20 -s 100000                  # Run for 20 seconds with 100KB images"
    echo "  $0 -r 10Mbps -d 5ms -n              # High speed link without NetAnim"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--time)
            SIMULATION_TIME="$2"
            shift 2
            ;;
        -s|--image-size)
            IMAGE_SIZE="$2"
            shift 2
            ;;
        -p|--packet-size)
            PACKET_SIZE="$2"
            shift 2
            ;;
        -r|--data-rate)
            DATA_RATE="$2"
            shift 2
            ;;
        -d|--delay)
            DELAY="$2"
            shift 2
            ;;
        -n|--no-netanim)
            ENABLE_NETANIM=false
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Check if we're in the correct directory (should contain wscript)
if [ ! -f "wscript" ]; then
    echo "Error: wscript not found. Please run this script from the project directory."
    exit 1
fi

# Check if NS-3 is properly configured
if [ ! -d "../src" ]; then
    echo "Error: This doesn't appear to be inside an NS-3 directory."
    echo "Please ensure this project is inside your NS-3 installation."
    exit 1
fi

echo "Simulation Parameters:"
echo "  Simulation Time: $SIMULATION_TIME seconds"
echo "  Image Size: $IMAGE_SIZE bytes"
echo "  Packet Size: $PACKET_SIZE bytes"
echo "  Data Rate: $DATA_RATE"
echo "  Link Delay: $DELAY"
echo "  NetAnim: $ENABLE_NETANIM"
echo ""

# Build the simulation
echo "Building simulation..."
cd ..
./ns3 configure --enable-examples --enable-tests
./ns3 build

if [ $? -ne 0 ]; then
    echo "Error: Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""

# Run the simulation
echo "Running simulation..."
PROJECT_DIR=$(basename "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")

./ns3 run "$PROJECT_DIR/footballer-network-sim --simulationTime=$SIMULATION_TIME --imageSize=$IMAGE_SIZE --packetSize=$PACKET_SIZE --dataRate=$DATA_RATE --delay=$DELAY --enableNetAnim=$ENABLE_NETANIM"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Simulation completed successfully! ==="
    
    # Check if NetAnim file was generated
    if [ "$ENABLE_NETANIM" = true ] && [ -f "footballer-network-anim.xml" ]; then
        echo "NetAnim file generated: footballer-network-anim.xml"
        echo "You can visualize the network using: netanim footballer-network-anim.xml"
    fi
    
    # Check for any output files
    if [ -f "flowmon-results.xml" ]; then
        echo "Flow monitor results saved to: flowmon-results.xml"
    fi
else
    echo "Error: Simulation failed!"
    exit 1
fi
