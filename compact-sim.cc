// Footballer Network Sim - Compact Version
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include <map>
#include <vector>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("FootballSim");

enum FootballerID {MESSI=0,RONALDO,NEYMAR,MBAPPE,HAALAND};
std::map<FootballerID, std::string> names = {{MESSI,"Messi"},{RONALDO,"Ronaldo"},{NEYMAR,"Neymar"},{MBAPPE,"Mbappe"},{HAALAND,"Haaland"}};

class CompactSender : public Application {
public:
    void Setup(Address addr, uint32_t sz, DataRate rate, FootballerID id) { m_peer=addr; m_size=sz; m_rate=rate; m_id=id; }
private:
    void StartApplication() override { 
        m_sock = Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); 
        m_sock->Connect(m_peer); SendPkt(); 
    }
    void SendPkt() { 
        auto pkt = Create<Packet>(m_size); 
        m_sock->Send(pkt); 
        if(++m_sent < 10) Simulator::Schedule(Seconds(0.1), &CompactSender::SendPkt, this); 
    }
    Ptr<Socket> m_sock; Address m_peer; uint32_t m_size=1024, m_sent=0; DataRate m_rate; FootballerID m_id;
};

class CompactReceiver : public Application {
public:
    void Setup(uint16_t port, std::string name) { m_port=port; m_name=name; }
private:
    void StartApplication() override { 
        m_sock = Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); 
        m_sock->Bind(InetSocketAddress(Ipv4Address::GetAny(),m_port)); 
        m_sock->SetRecvCallback(MakeCallback(&CompactReceiver::Recv,this)); 
    }
    void Recv(Ptr<Socket> sock) { 
        Ptr<Packet> pkt; Address from; 
        while((pkt=sock->RecvFrom(from))) NS_LOG_INFO(m_name << " got packet"); 
    }
    Ptr<Socket> m_sock; uint16_t m_port; std::string m_name; 
};

int main() {
    LogComponentEnable("FootballSim", LOG_LEVEL_INFO);
    
    // Create nodes
    NodeContainer nodes; nodes.Create(6);
    
    // Setup network
    PointToPointHelper p2p; p2p.SetDeviceAttribute("DataRate",StringValue("5Mbps")); p2p.SetChannelAttribute("Delay",StringValue("2ms"));
    InternetStackHelper stack; stack.Install(nodes);
    Ipv4AddressHelper addr; 
    
    // Connect & assign IPs
    std::vector<Ipv4InterfaceContainer> ints;
    for(uint32_t i=0; i<5; ++i) {
        auto devs = p2p.Install(nodes.Get(0), nodes.Get(i+1));
        addr.SetBase(("10.1." + std::to_string(i+1) + ".0").c_str(), "255.255.255.0");
        ints.push_back(addr.Assign(devs));
    }
    
    // Install apps
    for(uint32_t i=0; i<5; ++i) {
        auto rcv = CreateObject<CompactReceiver>(); rcv->Setup(8080, names[static_cast<FootballerID>(i)]);
        nodes.Get(i+1)->AddApplication(rcv); rcv->SetStartTime(Seconds(1)); rcv->SetStopTime(Seconds(10));
        
        auto snd = CreateObject<CompactSender>(); snd->Setup(InetSocketAddress(ints[i].GetAddress(1),8080), 1024, DataRate("1Mbps"), static_cast<FootballerID>(i));
        nodes.Get(0)->AddApplication(snd); snd->SetStartTime(Seconds(2+i*0.1)); snd->SetStopTime(Seconds(10));
    }
    
    // Animation with proper positioning
    AnimationInterface anim("compact-football.xml");
    anim.UpdateNodeDescription(nodes.Get(0), "S");
    for(uint32_t i=1; i<6; ++i) anim.UpdateNodeDescription(nodes.Get(i), names[static_cast<FootballerID>(i-1)]);
    
    // Set positions: Sender left, Receivers in semicircle on right
    anim.SetConstantPosition(nodes.Get(0), 10, 50); // Sender on left
    anim.SetConstantPosition(nodes.Get(1), 70, 20); // R1 (Mbappe)
    anim.SetConstantPosition(nodes.Get(2), 80, 35); // R2 (Haaland) 
    anim.SetConstantPosition(nodes.Get(3), 80, 50); // R3 (Messi)
    anim.SetConstantPosition(nodes.Get(4), 80, 65); // R4 (Ronaldo)
    anim.SetConstantPosition(nodes.Get(5), 70, 80); // R5 (Neymar)
    
    Simulator::Stop(Seconds(11)); Simulator::Run(); Simulator::Destroy();
    NS_LOG_INFO("Done!"); return 0;
}
