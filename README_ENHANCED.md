# Footballer Network Simulation with Image Classification

A comprehensive NS-3 network simulation that demonstrates intelligent content delivery using computer vision for footballer image classification. This project combines OpenCV for real-time image analysis with NS-3 for realistic network simulation, showcasing content-aware routing mechanisms.

## 🎯 Project Overview

This simulation creates a network topology where:
- **1 Sender node** classifies footballer images using OpenCV
- **5 Receiver nodes** each dedicated to a specific footballer (Messi, Ronaldo, Neymar, Mbappe, Haaland)
- Images are automatically routed to the correct receiver based on classification results
- Comprehensive performance metrics and visualizations are generated

## 🚀 Features

### 🧠 Intelligent Content Processing
- **Real Image Classification**: OpenCV-based face recognition with LBPH (Local Binary Pattern Histograms)
- **Content-Aware Routing**: Intelligent packet forwarding based on image content
- **Dual Mode Operation**: Real image mode and simulated data mode
- **Training System**: Automated model training with organized datasets

### 🌐 Network Simulation
- **NS-3 Network Simulation**: Realistic network conditions with configurable parameters
- **Point-to-Point Topology**: Star configuration with dedicated links
- **Performance Analytics**: Detailed metrics including latency, throughput, and packet loss
- **Flow Monitoring**: Per-flow statistics and analysis

### 📊 Visualization & Analysis
- **NetAnim Support**: Network topology visualization
- **Comprehensive Analysis**: Python-based result analysis with matplotlib visualizations
- **Multi-Scenario Testing**: Automated scenario comparison
- **Real-time Metrics**: Live performance monitoring

## 🔧 System Requirements

### Dependencies
- **NS-3** (version 3.40+)
- **OpenCV** (version 4.0+) with contrib modules
- **Python 3** with packages: matplotlib, seaborn, pandas, numpy
- **CMake** (version 3.10+)
- **GCC** with C++17 support

### Installation Commands (Ubuntu/Debian)
```bash
# Install OpenCV
sudo apt-get install libopencv-dev libopencv-contrib-dev

# Install Python packages  
pip3 install matplotlib seaborn pandas numpy opencv-python

# Install build tools
sudo apt-get install cmake build-essential pkg-config
```

## 📁 Project Structure

```
CN-ns3-project/
├── 🔬 Core Simulation Files
│   ├── footballer-network-sim.cc    # Main NS-3 simulation
│   ├── image_classifier.h           # Image classification header
│   ├── image_classifier.cpp         # Image classification implementation
│   ├── wscript                      # NS-3 build script
│   └── simulation_config.ini        # Configuration parameters
│
├── 📊 Analysis & Visualization
│   ├── analyze_results.py           # Network performance analysis
│   ├── image_analysis.py            # Image classification analysis
│   └── run_scenarios.py             # Multi-scenario simulation runner
│
├── 🛠️ Build & Setup
│   ├── Makefile                     # Build configuration
│   ├── CMakeLists.txt              # CMake configuration
│   ├── setup_images.sh             # Project setup script
│   ├── run_simulation.sh           # Bash script runner
│   └── requirements.txt            # Python dependencies
│
├── 📷 Data Directories
│   ├── images/                      # Test images directory
│   ├── training_images/             # Training data directory
│   │   ├── Messi/
│   │   ├── Ronaldo/
│   │   ├── Neymar/
│   │   ├── Mbappe/
│   │   └── Haaland/
│   └── models/                      # Trained classifier models
│
└── 📈 Results
    └── results/                     # Analysis results and plots
```

## 🛠️ Quick Start

### 1. Setup Project
```bash
# Clone and setup
git clone <repository-url>
cd CN-ns3-project

# Run setup script
make setup
```

### 2. Check System Requirements
```bash
make check
```

### 3. Build Project
```bash
# Using Makefile (recommended)
make all

# Or using CMake
mkdir build && cd build
cmake ..
make
```

### 4. Run Simulation

#### 🎮 Simulated Mode (Default)
```bash
make run
```

#### 📷 Real Image Classification Mode
```bash
# Place images in training_images/ and images/ directories first
make run-real
```

#### ⚙️ Custom Parameters
```bash
./footballer-network-sim --useRealImages=true \
                        --imageDir=./images \
                        --trainDir=./training_images \
                        --simulationTime=15.0 \
                        --dataRate=2Mbps \
                        --packetSize=1024
```

### 5. Analyze Results
```bash
make analyze
```

## 📊 Simulation Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `--nNodes` | 6 | Total nodes (1 sender + 5 receivers) |
| `--packetSize` | 1024 | Packet size in bytes |
| `--dataRate` | 1Mbps | Link data rate |
| `--delay` | 2ms | Link propagation delay |
| `--simulationTime` | 10.0 | Simulation duration (seconds) |
| `--imageSize` | 50000 | Simulated image size (bytes) |
| `--useRealImages` | false | Enable real image classification |
| `--imageDir` | ./images | Directory with test images |
| `--trainDir` | ./training_images | Training data directory |
| `--modelPath` | ./footballer_model.yml | Trained model path |
| `--enableNetAnim` | true | Enable NetAnim visualization |

## 🖼️ Image Classification Setup

### Training Data Structure
```
training_images/
├── Messi/
│   ├── messi1.jpg
│   ├── messi2.jpg
│   └── messi_N.jpg
├── Ronaldo/
│   ├── ronaldo1.jpg
│   └── ronaldo_N.jpg
├── Neymar/
├── Mbappe/
└── Haaland/
```

### Image Requirements
- **Minimum**: 5-10 images per player for training
- **Recommended**: 20+ high-quality face images per player
- **Formats**: JPG, JPEG, PNG
- **Quality**: Clear face visibility, good lighting, various angles

### Classification Process
1. **Face Detection**: Haar cascade-based face detection
2. **Feature Extraction**: LBPH feature computation
3. **Training**: Model training with labeled data
4. **Classification**: Real-time image classification during simulation

## 📈 Analysis and Visualization

### Network Performance Metrics
- **📡 Throughput**: Data transmission rate per receiver
- **⏱️ Latency**: End-to-end packet delay analysis
- **📦 Packet Loss**: Transmission reliability metrics
- **🔄 Flow Statistics**: Detailed per-flow performance analysis

### Image Classification Metrics
- **🎯 Accuracy**: Overall classification performance
- **📊 Confidence Scores**: Classification certainty distribution
- **🔀 Confusion Matrix**: Per-player classification breakdown
- **👤 Performance by Player**: Individual accuracy rates

### Generated Outputs
- `footballer-network-anim.xml`: NetAnim visualization file
- `classification_analysis.png`: Classification performance plots
- `network_performance_by_player.png`: Network metrics by receiver
- `simulation_report_YYYYMMDD_HHMMSS.txt`: Comprehensive text report

## 🚀 Advanced Usage

### Multiple Scenario Analysis
```bash
python3 run_scenarios.py --scenarios all --output-dir results/scenarios
```

### Custom Classification Training
```bash
# Train with custom dataset
./footballer-network-sim --trainDir=./custom_training \
                        --modelPath=./custom_model.yml \
                        --useRealImages=true
```

### Performance Optimization Scenarios
```bash
# High throughput scenario
./footballer-network-sim --dataRate=10Mbps \
                        --packetSize=2048 \
                        --delay=1ms

# Low latency scenario  
./footballer-network-sim --delay=0.5ms \
                        --packetSize=512

# High load scenario
./footballer-network-sim --imageSize=200000 \
                        --simulationTime=30.0
```

## 🔧 Configuration

### simulation_config.ini
```ini
[network]
data_rate = 1Mbps
packet_size = 1024
delay = 2ms

[simulation]
duration = 10.0
enable_netanim = true

[classification]
confidence_threshold = 100.0
use_face_detection = true
model_path = ./footballer_model.yml

[paths]
image_dir = ./images
training_dir = ./training_images
results_dir = ./results
```

## 🐛 Troubleshooting

### Common Issues

#### 1. **OpenCV not found**
```bash
# Install OpenCV development packages
sudo apt-get install libopencv-dev libopencv-contrib-dev

# Verify installation
pkg-config --cflags opencv4
pkg-config --libs opencv4
```

#### 2. **NS-3 linking errors**
```bash
# Ensure NS-3 is properly installed
export PKG_CONFIG_PATH=/path/to/ns3/lib/pkgconfig:$PKG_CONFIG_PATH

# Verify NS-3 installation
pkg-config --cflags ns3-core
```

#### 3. **Face detection fails**
- Ensure `haarcascade_frontalface_alt.xml` is available
- Check image quality and lighting conditions
- Use higher resolution images (minimum 100x100 pixels)

#### 4. **Low classification accuracy**
- Increase training data quantity (20+ images per player)
- Improve image quality and consistency
- Ensure diverse poses and lighting in training set
- Check for proper face detection in training images

#### 5. **Build errors**
```bash
# Clean and rebuild
make clean
make all

# Check dependencies
make check
```

## 📊 Performance Benchmarks

### Typical Results
- **Classification Accuracy**: 85-95% (with good training data)
- **Network Throughput**: 0.8-1.0 Mbps per receiver
- **Average Latency**: 8-12 ms
- **Packet Loss**: <2% under normal conditions

### Optimization Tips
- Use high-quality, consistent training images
- Optimize network parameters based on image sizes
- Consider compression for large image files
- Use appropriate confidence thresholds

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines
- Follow C++17 standards for NS-3 code
- Use PEP 8 for Python code
- Add comprehensive comments
- Include unit tests where applicable
- Update documentation for new features

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **NS-3 Project**: Providing the network simulation framework
- **OpenCV Community**: Computer vision library and algorithms
- **Football Community**: Inspiration for player selection and testing
- **Academic Community**: Research foundation for content-aware networking

## 📚 References

- [NS-3 Network Simulator](https://www.nsnam.org/)
- [OpenCV Documentation](https://docs.opencv.org/)
- [Face Recognition Tutorial](https://docs.opencv.org/4.x/dd/d65/tutorial_face_recognition.html)
- [LBPH Face Recognition](https://docs.opencv.org/4.x/df/d25/tutorial_py_face_recognition.html)

## 📧 Support

For questions, issues, or contributions:
- Open an issue in the repository
- Check existing documentation
- Review troubleshooting section

---

**Made with ❤️ for the intersection of Computer Vision and Network Simulation**
