# Football Network Simulation Project

A comprehensive NS-3 based network simulation that demonstrates intelligent content delivery for footballer images using targeted transmission mechanisms.

## 🏈 Project Overview

This project implements a network simulation where:
- **1 Sender Node**: Transmits footballer images after classification
- **5 Receiver Nodes**: Each dedicated to a specific footballer (Messi, Ronaldo, Neymar, Mbappe, Haaland)
- **Smart Routing**: Images are routed to the correct receiver based on classification
- **Performance Analysis**: Comprehensive metrics collection and visualization
- **Network Visualization**: NetAnim integration for visual network analysis

## 🚀 Features

### Core Functionality
- ✅ **Network Topology**: Star topology with 1 sender and 5 receivers
- ✅ **Packetized Transmission**: Images converted to network packets
- ✅ **Targeted Delivery**: Each receiver assigned to specific footballer
- ✅ **Performance Metrics**: Latency, throughput, packet loss analysis
- ✅ **Visual Simulation**: NetAnim integration for network visualization

### Analysis & Reporting
- ✅ **Automated Analysis**: Python-based result analysis
- ✅ **Performance Plots**: Comprehensive visualization of metrics
- ✅ **CSV Export**: Detailed data export for further analysis
- ✅ **Scenario Comparison**: Multi-scenario performance comparison

### Simulation Scenarios
- ✅ **Multiple Scenarios**: Predefined network conditions
- ✅ **Configurable Parameters**: Flexible simulation settings
- ✅ **Batch Processing**: Automated scenario execution
- ✅ **Results Management**: Organized output structure

## 📁 Project Structure

```
CN-ns3-project/
├── footballer-network-sim.cc   # Main NS-3 simulation code
├── wscript                      # NS-3 build script
├── run_simulation.sh           # Bash script for running simulations
├── analyze_results.py          # Python analysis script
├── run_scenarios.py            # Scenario runner script
├── simulation_config.ini       # Configuration file with scenarios
├── Makefile                    # Build and run automation
├── README.md                   # This file
├── LICENSE                     # Project license
└── results/                    # Generated results (created after runs)
    ├── performance_analysis.png
    ├── simulation_results.csv
    └── performance_report.txt
```

## 🛠️ Prerequisites

### System Requirements
- **NS-3**: Version 3.40 or later
- **C++ Compiler**: GCC 7+ or Clang 6+
- **Python 3**: Version 3.6 or later
- **Make**: Build automation
- **Git**: Version control

### NS-3 Modules Required
- core
- network
- internet
- point-to-point
- applications
- netanim
- mobility
- flow-monitor

### Python Dependencies
```bash
pip install matplotlib pandas numpy configparser
```

## 🚀 Quick Start

### 1. Setup
```bash
# Clone the repository into your NS-3 directory
cd /path/to/ns-3.xx/
git clone <repository-url> CN-ns3-project

# Install Python dependencies
make install-deps
```

### 2. Build and Run
```bash
# Build the simulation
make build

# Run basic simulation
make run

# Run with custom parameters
make run ARGS='--simulationTime=20 --dataRate=10Mbps'
```

### 3. Analyze Results
```bash
# Analyze the last simulation
make analyze

# View NetAnim visualization (if available)
make netanim
```

## 📊 Usage Examples

### Basic Simulation
```bash
# Run with default parameters
make run
```

### Custom Parameters
```bash
# High bandwidth scenario
make custom-run TIME=15 SIZE=100000 RATE=10Mbps DELAY=1ms

# Low bandwidth scenario  
make custom-run TIME=25 SIZE=50000 RATE=256Kbps DELAY=50ms
```

### Scenario-Based Testing
```bash
# List available scenarios
make list-scenarios

# Run specific scenario
make scenario NAME=scenario_high_bandwidth

# Run all scenarios
make scenarios
```

### Direct NS-3 Execution
```bash
cd ..
./ns3 run "CN-ns3-project/footballer-network-sim --simulationTime=10 --imageSize=50000 --dataRate=1Mbps"
```

## ⚙️ Configuration

### Simulation Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `simulationTime` | 10.0 | Simulation duration (seconds) |
| `imageSize` | 50000 | Simulated image size (bytes) |
| `packetSize` | 1024 | Network packet size (bytes) |
| `dataRate` | 1Mbps | Link data rate |
| `delay` | 2ms | Link propagation delay |
| `enableNetAnim` | true | Enable NetAnim output |

### Predefined Scenarios

1. **scenario_basic**: Standard network conditions
2. **scenario_high_bandwidth**: 10Mbps, 1ms delay
3. **scenario_low_bandwidth**: 256Kbps, 10ms delay
4. **scenario_high_latency**: 2Mbps, 250ms delay (satellite-like)
5. **scenario_mobile_network**: 5Mbps, 50ms delay
6. **scenario_congested_network**: 512Kbps, 15ms delay
7. **scenario_real_time**: 50Mbps, 0.5ms delay

## 📈 Performance Metrics

### Collected Metrics
- **Packet Statistics**: Sent/received packet counts
- **Throughput**: Data transmission rate (Mbps)
- **End-to-End Delay**: Average packet delay (seconds)
- **Packet Loss Rate**: Percentage of lost packets
- **Jitter**: Delay variation (seconds)
- **Delivery Efficiency**: Success rate per footballer

### Analysis Output
- **Text Report**: Comprehensive performance summary
- **CSV Data**: Detailed metrics for further analysis
- **Visualizations**: Performance comparison charts
- **NetAnim File**: Network animation for visual analysis

## 🎯 Network Topology

```
                    Sender (Node 0)
                         |
        ┌────────────────┼────────────────┐
        │                │                │
   Receiver 1        Receiver 2      Receiver 3
   (Messi)          (Ronaldo)        (Neymar)
        │                │                │
        └────────────────┼────────────────┘
                         │
                ┌────────┴────────┐
           Receiver 4         Receiver 5
           (Mbappe)          (Haaland)
```

## 🔧 Development

### Adding New Scenarios
1. Edit `simulation_config.ini`
2. Add new `[scenario_name]` section
3. Define parameters and description
4. Run with `make scenario NAME=scenario_name`

### Customizing Analysis
- Modify `analyze_results.py` for custom metrics
- Add new visualization functions
- Extend CSV export fields

### Code Structure
- **ImageSenderApp**: Custom application for image transmission
- **ImageReceiverApp**: Custom application for image reception
- **Network Setup**: Point-to-point topology configuration
- **Metrics Collection**: Flow monitor and custom statistics

## 🐛 Troubleshooting

### Common Issues

1. **Build Errors**
   ```bash
   # Clean and rebuild
   make clean
   make build
   ```

2. **Missing Dependencies**
   ```bash
   # Check dependencies
   make check-deps
   
   # Install Python packages
   make install-deps
   ```

3. **No Results Generated**
   - Ensure NS-3 is properly installed
   - Check that simulation runs without errors
   - Verify file permissions

4. **NetAnim Issues**
   - Install NetAnim separately if needed
   - Check if XML file is generated
   - Verify NetAnim compatibility

### Debug Mode
```bash
# Run with detailed logging
cd ..
./ns3 run "CN-ns3-project/footballer-network-sim" --verbose
```

## 📊 Sample Results

### Performance Summary
```
=== SIMULATION RESULTS ===
Simulation Time: 10 seconds
Image Size: 50000 bytes
Data Rate: 1Mbps

--- Per-Node Statistics ---
Node 1 (Messi Receiver): Packets Received: 49, Avg Latency: 0.0234 seconds
Node 2 (Ronaldo Receiver): Packets Received: 49, Avg Latency: 0.0235 seconds
Node 3 (Neymar Receiver): Packets Received: 49, Avg Latency: 0.0233 seconds
Node 4 (Mbappe Receiver): Packets Received: 49, Avg Latency: 0.0236 seconds
Node 5 (Haaland Receiver): Packets Received: 49, Avg Latency: 0.0232 seconds
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **NS-3 Network Simulator**: Foundation for network simulation
- **OpenCV Community**: Image processing inspiration
- **Football Community**: Player inspiration

## 📞 Support

For questions and support:
- Check the [troubleshooting section](#-troubleshooting)
- Review NS-3 documentation
- Open an issue for bugs or feature requests

---

**Note**: This project focuses on the network simulation aspects. The image classification component should be integrated separately using OpenCV or similar computer vision libraries.
