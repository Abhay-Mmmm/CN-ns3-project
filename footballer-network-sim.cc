/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Football Image Network Transmission Simulation
 * 
 * This simulation demonstrates intelligent content delivery where:
 * - 1 Sender node transmits footballer images
 * - 5 Receiver nodes, each assigned to a specific footballer
 * - Images are packetized and transmitted through NS-3 network
 * - Performance metrics are collected and analyzed
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FootballerNetworkSim");

// Footballer to receiver mapping
enum FootballerID {
    MESSI = 0,
    RONALDO = 1,
    NEYMAR = 2,
    MBAPPE = 3,
    HAALAND = 4
};

std::map<FootballerID, std::string> footballerNames = {
    {MESSI, "Messi"},
    {RONALDO, "Ronaldo"},
    {NEYMAR, "Neymar"},
    {MBAPPE, "Mbappe"},
    {HAALAND, "Haaland"}
};

// Global variables for statistics
std::map<uint32_t, uint32_t> packetsReceived;
std::map<uint32_t, double> totalLatency;
std::map<uint32_t, uint32_t> packetsSent;
Time simulationStartTime;

// Custom application for image transmission
class ImageSenderApp : public Application
{
public:
    ImageSenderApp();
    virtual ~ImageSenderApp();
    
    void Setup(Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, FootballerID playerId);
    void SetImageData(const std::vector<uint8_t>& imageData);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    
    void ScheduleTransmit(Time dt);
    void SendPacket(void);
    
    Ptr<Socket> m_socket;
    Address m_peer;
    uint32_t m_packetSize;
    uint32_t m_nPackets;
    DataRate m_dataRate;
    EventId m_sendEvent;
    bool m_running;
    uint32_t m_packetsSent;
    FootballerID m_playerId;
    std::vector<uint8_t> m_imageData;
    uint32_t m_currentByte;
};

ImageSenderApp::ImageSenderApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0),
      m_currentByte(0)
{
}

ImageSenderApp::~ImageSenderApp()
{
    m_socket = 0;
}

void ImageSenderApp::Setup(Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, FootballerID playerId)
{
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_playerId = playerId;
}

void ImageSenderApp::SetImageData(const std::vector<uint8_t>& imageData)
{
    m_imageData = imageData;
    // Calculate number of packets needed
    m_nPackets = (imageData.size() + m_packetSize - 1) / m_packetSize;
}

void ImageSenderApp::StartApplication(void)
{
    m_running = true;
    m_packetsSent = 0;
    m_currentByte = 0;
    m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
    m_socket->Bind();
    m_socket->Connect(m_peer);
    
    SendPacket();
}

void ImageSenderApp::StopApplication(void)
{
    m_running = false;
    
    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }
    
    if (m_socket)
    {
        m_socket->Close();
    }
}

void ImageSenderApp::SendPacket(void)
{
    if (!m_running || m_currentByte >= m_imageData.size())
        return;
    
    // Create packet with image data
    uint32_t bytesToSend = std::min(m_packetSize, (uint32_t)(m_imageData.size() - m_currentByte));
    Ptr<Packet> packet = Create<Packet>(&m_imageData[m_currentByte], bytesToSend);
    
    // Add custom header with footballer ID and sequence number
    uint32_t header = (m_playerId << 16) | (m_packetsSent & 0xFFFF);
    packet->AddPaddingAtEnd(4); // Space for our header
    
    m_socket->Send(packet);
    m_currentByte += bytesToSend;
    
    ++m_packetsSent;
    packetsSent[GetNode()->GetId()]++;
    
    NS_LOG_INFO("Sent packet " << m_packetsSent << " for " << footballerNames[m_playerId] 
                << " (size: " << bytesToSend << " bytes)");
    
    if (m_currentByte < m_imageData.size())
    {
        ScheduleTransmit(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
    }
}

void ImageSenderApp::ScheduleTransmit(Time dt)
{
    if (m_running)
    {
        m_sendEvent = Simulator::Schedule(dt, &ImageSenderApp::SendPacket, this);
    }
}

// Custom application for image reception
class ImageReceiverApp : public Application
{
public:
    ImageReceiverApp();
    virtual ~ImageReceiverApp();
    
    void Setup(uint16_t port, FootballerID expectedPlayer);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    
    void HandleRead(Ptr<Socket> socket);
    
    Ptr<Socket> m_socket;
    uint16_t m_port;
    FootballerID m_expectedPlayer;
    std::vector<uint8_t> m_receivedData;
    std::map<uint32_t, Time> m_packetTimestamps;
};

ImageReceiverApp::ImageReceiverApp()
    : m_socket(0),
      m_port(0)
{
}

ImageReceiverApp::~ImageReceiverApp()
{
    m_socket = 0;
}

void ImageReceiverApp::Setup(uint16_t port, FootballerID expectedPlayer)
{
    m_port = port;
    m_expectedPlayer = expectedPlayer;
}

void ImageReceiverApp::StartApplication(void)
{
    m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), m_port);
    m_socket->Bind(local);
    m_socket->SetRecvCallback(MakeCallback(&ImageReceiverApp::HandleRead, this));
}

void ImageReceiverApp::StopApplication(void)
{
    if (m_socket != 0)
    {
        m_socket->Close();
        m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    }
}

void ImageReceiverApp::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    
    while ((packet = socket->RecvFrom(from)))
    {
        Time currentTime = Simulator::Now();
        
        // Extract our custom header (simplified)
        uint32_t packetSize = packet->GetSize();
        if (packetSize >= 4)
        {
            // Remove our header padding
            packet->RemoveAtEnd(4);
        }
        
        // Update statistics
        packetsReceived[GetNode()->GetId()]++;
        
        // Calculate latency (simplified - in real implementation, timestamp would be in packet)
        double latency = (currentTime - simulationStartTime).GetSeconds();
        totalLatency[GetNode()->GetId()] += latency;
        
        NS_LOG_INFO("Node " << GetNode()->GetId() << " (" << footballerNames[m_expectedPlayer] 
                    << " receiver) received packet of size " << packet->GetSize() 
                    << " (Total packets: " << packetsReceived[GetNode()->GetId()] << ")");
        
        // Store packet data
        uint8_t* buffer = new uint8_t[packet->GetSize()];
        packet->CopyData(buffer, packet->GetSize());
        m_receivedData.insert(m_receivedData.end(), buffer, buffer + packet->GetSize());
        delete[] buffer;
    }
}

// Function to simulate image data (since we're not using actual classifier)
std::vector<uint8_t> GenerateSimulatedImageData(FootballerID playerId, uint32_t imageSize)
{
    std::vector<uint8_t> imageData(imageSize);
    
    // Fill with pattern based on player ID (simulating different image content)
    uint8_t pattern = static_cast<uint8_t>(playerId * 50 + 10);
    for (uint32_t i = 0; i < imageSize; ++i)
    {
        imageData[i] = (pattern + i) % 256;
    }
    
    return imageData;
}

int main(int argc, char *argv[])
{
    CommandLine cmd;
    
    // Simulation parameters
    uint32_t nNodes = 6; // 1 sender + 5 receivers
    uint32_t packetSize = 1024; // bytes
    std::string dataRate = "1Mbps";
    std::string delay = "2ms";
    double simulationTime = 10.0; // seconds
    uint32_t imageSize = 50000; // bytes (simulated image size)
    bool enableNetAnim = true;
    
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("packetSize", "Packet size in bytes", packetSize);
    cmd.AddValue("dataRate", "Data rate", dataRate);
    cmd.AddValue("delay", "Link delay", delay);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("imageSize", "Simulated image size in bytes", imageSize);
    cmd.AddValue("enableNetAnim", "Enable NetAnim output", enableNetAnim);
    
    cmd.Parse(argc, argv);
    
    // Initialize simulation start time
    simulationStartTime = Simulator::Now();
    
    NS_LOG_INFO("Creating " << nNodes << " nodes.");
    
    // Create nodes
    NodeContainer nodes;
    nodes.Create(nNodes);
    
    // Create point-to-point helper
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(dataRate));
    pointToPoint.SetChannelAttribute("Delay", StringValue(delay));
    
    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);
    
    // Create network devices and assign IP addresses
    NetDeviceContainer devices;
    Ipv4AddressHelper address;
    
    // Connect sender (node 0) to all receivers (nodes 1-5)
    for (uint32_t i = 1; i < nNodes; ++i)
    {
        NodeContainer link = NodeContainer(nodes.Get(0), nodes.Get(i));
        NetDeviceContainer linkDevices = pointToPoint.Install(link);
        devices.Add(linkDevices);
        
        std::ostringstream subnet;
        subnet << "10.1." << i << ".0";
        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign(linkDevices);
    }
    
    // Set up mobility for NetAnim visualization
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    
    // Position sender at center
    positionAlloc->Add(Vector(50.0, 50.0, 0.0));
    
    // Position receivers in a circle around sender
    double radius = 30.0;
    for (uint32_t i = 1; i < nNodes; ++i)
    {
        double angle = 2.0 * M_PI * (i - 1) / (nNodes - 1);
        double x = 50.0 + radius * cos(angle);
        double y = 50.0 + radius * sin(angle);
        positionAlloc->Add(Vector(x, y, 0.0));
    }
    
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    
    // Install receiver applications
    ApplicationContainer receiverApps;
    uint16_t port = 9;
    
    for (uint32_t i = 1; i < nNodes; ++i)
    {
        FootballerID playerId = static_cast<FootballerID>(i - 1);
        
        Ptr<ImageReceiverApp> receiverApp = CreateObject<ImageReceiverApp>();
        receiverApp->Setup(port, playerId);
        nodes.Get(i)->AddApplication(receiverApp);
        receiverApp->SetStartTime(Seconds(1.0));
        receiverApp->SetStopTime(Seconds(simulationTime));
        receiverApps.Add(receiverApp);
        
        NS_LOG_INFO("Installed receiver for " << footballerNames[playerId] 
                    << " on node " << i);
    }
    
    // Install sender applications (simulate sending to each receiver)
    ApplicationContainer senderApps;
    
    for (uint32_t i = 1; i < nNodes; ++i)
    {
        FootballerID playerId = static_cast<FootballerID>(i - 1);
        
        // Get receiver IP address
        std::ostringstream receiverIP;
        receiverIP << "10.1." << i << ".2";
        Address receiverAddress = InetSocketAddress(Ipv4Address(receiverIP.str().c_str()), port);
        
        Ptr<ImageSenderApp> senderApp = CreateObject<ImageSenderApp>();
        senderApp->Setup(receiverAddress, packetSize, 0, DataRate(dataRate), playerId);
        
        // Generate simulated image data for this player
        std::vector<uint8_t> imageData = GenerateSimulatedImageData(playerId, imageSize);
        senderApp->SetImageData(imageData);
        
        nodes.Get(0)->AddApplication(senderApp);
        senderApp->SetStartTime(Seconds(2.0 + i * 0.5)); // Stagger transmissions
        senderApp->SetStopTime(Seconds(simulationTime));
        senderApps.Add(senderApp);
        
        NS_LOG_INFO("Installed sender for " << footballerNames[playerId] 
                    << " image to node " << i);
    }
    
    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Set up Flow Monitor for detailed statistics
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    // Enable NetAnim
    if (enableNetAnim)
    {
        AnimationInterface anim("footballer-network-anim.xml");
        
        // Set node descriptions
        anim.UpdateNodeDescription(nodes.Get(0), "Sender");
        for (uint32_t i = 1; i < nNodes; ++i)
        {
            FootballerID playerId = static_cast<FootballerID>(i - 1);
            anim.UpdateNodeDescription(nodes.Get(i), footballerNames[playerId] + " Receiver");
        }
        
        // Set node colors
        anim.UpdateNodeColor(nodes.Get(0), 255, 0, 0); // Red for sender
        for (uint32_t i = 1; i < nNodes; ++i)
        {
            anim.UpdateNodeColor(nodes.Get(i), 0, 255, 0); // Green for receivers
        }
        
        NS_LOG_INFO("NetAnim configuration enabled. Output file: footballer-network-anim.xml");
    }
    
    NS_LOG_INFO("Starting simulation for " << simulationTime << " seconds...");
    
    // Run simulation
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
    // Print statistics
    std::cout << "\n=== SIMULATION RESULTS ===" << std::endl;
    std::cout << "Simulation Time: " << simulationTime << " seconds" << std::endl;
    std::cout << "Image Size: " << imageSize << " bytes" << std::endl;
    std::cout << "Packet Size: " << packetSize << " bytes" << std::endl;
    std::cout << "Data Rate: " << dataRate << std::endl;
    std::cout << "\n--- Per-Node Statistics ---" << std::endl;
    
    for (uint32_t i = 0; i < nNodes; ++i)
    {
        if (i == 0)
        {
            std::cout << "Node " << i << " (Sender): ";
            std::cout << "Packets Sent: " << packetsSent[i] << std::endl;
        }
        else
        {
            FootballerID playerId = static_cast<FootballerID>(i - 1);
            std::cout << "Node " << i << " (" << footballerNames[playerId] << " Receiver): ";
            std::cout << "Packets Received: " << packetsReceived[i];
            if (packetsReceived[i] > 0)
            {
                double avgLatency = totalLatency[i] / packetsReceived[i];
                std::cout << ", Avg Latency: " << avgLatency << " seconds";
            }
            std::cout << std::endl;
        }
    }
    
    // Flow Monitor statistics
    std::cout << "\n--- Flow Monitor Statistics ---" << std::endl;
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")" << std::endl;
        std::cout << "  Tx Packets: " << i->second.txPackets << std::endl;
        std::cout << "  Rx Packets: " << i->second.rxPackets << std::endl;
        std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024 << " Mbps" << std::endl;
        std::cout << "  Mean Delay: " << i->second.delaySum.GetSeconds() / i->second.rxPackets << " seconds" << std::endl;
        std::cout << "  Packet Loss Ratio: " << (double)(i->second.txPackets - i->second.rxPackets) / i->second.txPackets * 100 << "%" << std::endl;
    }
    
    Simulator::Destroy();
    
    std::cout << "\nSimulation completed successfully!" << std::endl;
    if (enableNetAnim)
    {
        std::cout << "NetAnim file generated: footballer-network-anim.xml" << std::endl;
    }
    
    return 0;
}
