# 🏈 Footballer Network Simulation Project

## 🎯 What is This Project?

This project demonstrates **intelligent content-aware networking** using a football theme. It simulates a network where:

- **A sender** transmits data packets containing footballer information
- **A classifier** analyzes each packet and routes it to the correct destination
- **Five receivers** each receive packets meant for their specific footballer:
  - **R1**: Mbappe packets
  - **R2**: Haaland packets  
  - **R3**: Messi packets
  - **R4**: Ronaldo packets
  - **R5**: Neymar packets

Think of it as a **smart mail sorting system** that automatically delivers footballer fan mail to the right person!

## 🌐 Network Architecture

```
📤 SENDER (S) ──────▶ 🤖 CLASSIFIER (C) ──────▶ 📨 RECEIVER 1 (Mbappe)
                                        ├──────▶ 📨 RECEIVER 2 (Haaland)
                                        ├──────▶ 📨 RECEIVER 3 (Messi)
                                        ├──────▶ 📨 RECEIVER 4 (Ronaldo)
                                        └──────▶ 📨 RECEIVER 5 (Neymar)
```

## 🛠️ Technologies Used

- **NS-3**: Network simulation framework (simulates real networks)
- **C++20**: Modern C++ for high performance
- **OpenCV**: Computer vision for image classification (future enhancement)
- **NetAnim**: Network visualization tool
- **UDP Sockets**: For packet transmission

## 📁 Project Structure

```
CN-ns3-project/
│
├── 🎯 CORE FILES
│   ├── compact-sim.cc              # Simple network demo
│   ├── compact-classifier.cc       # Main classifier simulation
│   ├── compact_classifier.h        # AI classification logic
│   └── Makefile-compact           # Build system
│
├── 📊 DATA & MODELS
│   ├── training_images/           # Training data for AI
│   │   ├── Messi/                # Messi training images
│   │   ├── Ronaldo/              # Ronaldo training images
│   │   ├── Neymar/               # Neymar training images
│   │   ├── Mbappe/               # Mbappe training images
│   │   └── Haaland/              # Haaland training images
│   ├── images/                   # Test images
│   └── models/                   # Pre-trained AI models
│
├── 📋 DOCUMENTATION
│   ├── README.md                 # This file
│   ├── README-compact.md         # Quick reference
│   └── compact_analysis.py       # Analysis tools
│
└── 📦 OUTPUT FILES
    ├── compact-classifier.xml    # Network animation
    └── compact-football.xml     # Simple animation
```

## 🚀 Quick Start Guide

### Prerequisites

You need:
- **Linux environment** (WSL on Windows works great)
- **NS-3.45** installed and built
- **g++** compiler with C++20 support

### Step 1: Build the Project

```bash
# Navigate to project directory
cd /path/to/CN-ns3-project

# Build both simulations
make -f Makefile-compact compact
```

### Step 2: Run the Simulation

```bash
# Option A: Run the classifier demo (recommended)
make -f Makefile-compact run-classifier

# Option B: Run the simple demo
make -f Makefile-compact run-sim
```

### Step 3: View the Animation

```bash
# Open NetAnim to see the network visualization
netanim compact-football.xml
```

## 🎮 What You'll See

### In the Terminal:
```
AnimationInterface WARNING: Node:0 Does not have a mobility model...
AnimationInterface WARNING: Node:1 Does not have a mobility model...
...
```
*(These warnings are normal - they just mean nodes are stationary)*

### In NetAnim:
- **Circles**: Network nodes (S, C, R1-R5)
- **Lines**: Network connections
- **Moving dots**: Data packets being transmitted
- **Labels**: Node descriptions (Sender, Classifier, Receivers)

## 🔍 How It Works (Technical Details)

### 1. Packet Creation
```cpp
// Sender creates packets with footballer IDs
FootballerHeader header;
header.SetId(footballerId);  // 0=Mbappe, 1=Haaland, etc.
packet->AddHeader(header);
```

### 2. Classification & Routing
```cpp
// Classifier reads the header and routes accordingly
FootballerHeader header;
packet->RemoveHeader(header);
uint8_t footballerId = header.GetId();
// Send to appropriate receiver based on ID
```

### 3. Packet Reception
```cpp
// Each receiver only gets packets meant for their footballer
if (receivedPacket.footballerId == myFootballerId) {
    processPacket();
}
```

## 📊 Network Performance

The simulation tracks:
- **Packets sent** by the sender
- **Packets received** by the classifier  
- **Packets forwarded** by the classifier
- **Packets received** by each receiver
- **End-to-end latency**
- **Network throughput**

## 🎯 Available Commands

### Build Commands
```bash
make -f Makefile-compact compact      # Build everything
make -f Makefile-compact clean        # Clean build files
```

### Run Commands
```bash
make -f Makefile-compact run-classifier  # Main simulation (S→C→R)
make -f Makefile-compact run-sim         # Simple simulation (S→R)
```

### Analysis Commands
```bash
python3 compact_analysis.py              # Basic analysis
python3 compact_analysis.py test         # Run all tests
```

## 🔧 Customization Options

### Change Number of Packets
Edit `compact-classifier.cc`:
```cpp
if(++m_cnt<25)  // Change 25 to your desired number
```

### Change Packet Size
```cpp
auto pkt=Create<Packet>(1024); // Change 1024 to your size
```

### Change Transmission Delay
```cpp
Simulator::Schedule(Seconds(0.1), ...); // Change 0.1 to your delay
```

### Add More Footballers
1. Add to enum: `enum FID {MBAPPE=0,HAALAND,MESSI,RONALDO,NEYMAR,YOURPLAYER};`
2. Update names array
3. Create additional receiver nodes

## 🐛 Troubleshooting

### Common Issues & Solutions

**1. "No such file or directory" error**
```bash
# Make sure you're in the right directory
cd /path/to/CN-ns3-project
pwd  # Should show your project path
```

**2. "Library not found" error**
```bash
# Set library path explicitly
LD_LIBRARY_PATH=../build/lib:$LD_LIBRARY_PATH ./compact-classifier
```

**3. "NS-3 not found" error**
```bash
# Make sure NS-3 is built
cd /path/to/ns-3.45
./ns3 configure --enable-examples
./ns3 build
```

**4. NetAnim doesn't open**
```bash
# Check if NetAnim is installed
which netanim

# If not installed:
sudo apt install netanim
```

## 🎓 Educational Value

### What This Project Teaches:

1. **Network Simulation**: How to model real networks in software
2. **Packet Switching**: How data moves through networks
3. **Content-Aware Routing**: Intelligent packet forwarding
4. **Protocol Design**: Creating custom network protocols
5. **Performance Analysis**: Measuring network metrics
6. **Visualization**: Making complex systems understandable

### Concepts Demonstrated:

- **UDP Socket Programming**: Basic network communication
- **Header Processing**: Adding metadata to packets
- **Routing Algorithms**: Deciding where to send data
- **Network Topology**: How nodes connect
- **Quality of Service**: Prioritizing different traffic types

## 🔮 Future Enhancements

This project can be extended with:

- **Real Image Classification**: Use OpenCV to classify actual footballer photos
- **Machine Learning**: Train AI models for better classification
- **Multiple Senders**: Add more sender nodes
- **Dynamic Routing**: Change routes based on network conditions
- **Performance Optimization**: Implement caching and load balancing
- **Security Features**: Add encryption and authentication
- **Real Hardware**: Deploy on actual network equipment

## 📚 Learning Resources

To understand this project better, study:

- **NS-3 Tutorial**: https://www.nsnam.org/documentation/
- **Computer Networks**: Tanenbaum & Wetherall book
- **Network Programming**: Beej's Guide to Network Programming
- **C++ Reference**: https://cppreference.com/
- **UDP Protocol**: RFC 768 specification

## 🤝 Contributing

Want to improve this project? Here's how:

1. **Fork** the repository
2. **Create** a feature branch
3. **Make** your improvements
4. **Test** thoroughly
5. **Submit** a pull request

Ideas for contributions:
- Add more detailed logging
- Implement error handling
- Create unit tests
- Add configuration files
- Improve documentation

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🆘 Getting Help

If you're stuck:

1. **Check the troubleshooting section** above
2. **Read the NS-3 documentation**: https://www.nsnam.org/
3. **Search for similar issues** on Stack Overflow
4. **Check the project issues** on GitHub

## 🎉 Conclusion

This footballer network simulation demonstrates how modern networks can intelligently route content based on its type. It's a fun, educational way to learn about:

- Network simulation and modeling
- Packet-based communication  
- Content-aware networking
- Performance analysis
- System visualization

The project successfully shows that even complex networking concepts can be made accessible and engaging through creative examples!

---

**Happy networking!** ⚽🌐🚀

*Built with ❤️ using NS-3 and modern C++*

