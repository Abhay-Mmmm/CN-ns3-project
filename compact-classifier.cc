// Compact Classifier Demo - S->C->R topology
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("CompactClassifier");

enum FID {MBAPPE=0,HAALAND,MESSI,RONALDO,NEYMAR};
std::vector<std::string> names={"Mbappe","Haaland","Messi","Ronaldo","Neymar"};

class FootballerHeader : public ns3::Header {
public:
    void SetId(uint8_t id) { m_id=id; } uint8_t GetId() const { return m_id; }
    static TypeId GetTypeId() { static TypeId tid=TypeId("FootballerHeader").SetParent<ns3::Header>().AddConstructor<FootballerHeader>(); return tid; }
    TypeId GetInstanceTypeId() const override { return GetTypeId(); }
    void Print(std::ostream &os) const override { os<<"ID="<<(uint32_t)m_id; }
    void Serialize(Buffer::Iterator start) const override { start.WriteU8(m_id); }
    uint32_t Deserialize(Buffer::Iterator start) override { m_id=start.ReadU8(); return 1; }
    uint32_t GetSerializedSize() const override { return 1; }
private: uint8_t m_id=0;
};

class Sender : public Application {
    void StartApplication() override { 
        m_sock=Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); m_sock->Connect(m_peer); Send(); 
    }
    void Send() { 
        auto pkt=Create<Packet>(1024); FootballerHeader h; h.SetId(m_cnt%5); pkt->AddHeader(h); m_sock->Send(pkt); 
        if(++m_cnt<25) Simulator::Schedule(Seconds(0.1),&Sender::Send,this); 
    }
public: void Setup(Address addr) { m_peer=addr; }
private: Ptr<Socket> m_sock; Address m_peer; uint32_t m_cnt=0;
};

class Classifier : public Application {
    void StartApplication() override { 
        m_rx=Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); 
        m_rx->Bind(InetSocketAddress(Ipv4Address::GetAny(),8080)); 
        m_rx->SetRecvCallback(MakeCallback(&Classifier::Recv,this));
        for(auto& a : m_addrs) { auto s=Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); s->Connect(a); m_tx.push_back(s); }
    }
    void Recv(Ptr<Socket> sock) { 
        Ptr<Packet> pkt; Address from; 
        while((pkt=sock->RecvFrom(from))) { FootballerHeader h; pkt->RemoveHeader(h); uint8_t id=h.GetId(); pkt->AddHeader(h); m_tx[id]->Send(pkt); }
    }
public: void Setup(std::vector<Address> addrs) { m_addrs=addrs; }
private: Ptr<Socket> m_rx; std::vector<Ptr<Socket>> m_tx; std::vector<Address> m_addrs;
};

class Receiver : public Application {
    void StartApplication() override { 
        m_sock=Socket::CreateSocket(GetNode(),UdpSocketFactory::GetTypeId()); 
        m_sock->Bind(InetSocketAddress(Ipv4Address::GetAny(),8081)); 
        m_sock->SetRecvCallback(MakeCallback(&Receiver::Recv,this)); 
    }
    void Recv(Ptr<Socket> sock) { Ptr<Packet> pkt; Address from; while((pkt=sock->RecvFrom(from))) ++m_cnt; }
public: void Setup(std::string n) { m_name=n; }
private: Ptr<Socket> m_sock; std::string m_name; uint32_t m_cnt=0;
};

int main() {
    NodeContainer s,c,r; s.Create(1); c.Create(1); r.Create(5);
    PointToPointHelper p2p; p2p.SetDeviceAttribute("DataRate",StringValue("5Mbps")); p2p.SetChannelAttribute("Delay",StringValue("2ms"));
    InternetStackHelper stack; stack.Install(s); stack.Install(c); stack.Install(r);
    
    auto sc=p2p.Install(s.Get(0),c.Get(0)); Ipv4AddressHelper addr; addr.SetBase("10.1.1.0","255.255.255.0"); auto sci=addr.Assign(sc);
    std::vector<Ipv4InterfaceContainer> cri; std::vector<Address> addrs;
    for(uint32_t i=0; i<5; ++i) {
        auto cr=p2p.Install(c.Get(0),r.Get(i)); addr.SetBase(("10.1."+ std::to_string(i+2)+".0").c_str(),"255.255.255.0");
        cri.push_back(addr.Assign(cr)); addrs.push_back(InetSocketAddress(cri[i].GetAddress(1),8081));
    }
    
    for(uint32_t i=0; i<5; ++i) { auto rcv=CreateObject<Receiver>(); rcv->Setup(names[i]); r.Get(i)->AddApplication(rcv); rcv->SetStartTime(Seconds(1)); }
    auto cls=CreateObject<Classifier>(); cls->Setup(addrs); c.Get(0)->AddApplication(cls); cls->SetStartTime(Seconds(1));
    auto snd=CreateObject<Sender>(); snd->Setup(InetSocketAddress(sci.GetAddress(1),8080)); s.Get(0)->AddApplication(snd); snd->SetStartTime(Seconds(2));
    
    AnimationInterface anim("compact-classifier.xml");
    anim.UpdateNodeDescription(s.Get(0),"S"); anim.UpdateNodeDescription(c.Get(0),"C");
    anim.UpdateNodeDescription(r.Get(0),"R1 (Mbappe)"); anim.UpdateNodeDescription(r.Get(1),"R2 (Haaland)");
    anim.UpdateNodeDescription(r.Get(2),"R3 (Messi)"); anim.UpdateNodeDescription(r.Get(3),"R4 (Ronaldo)"); 
    anim.UpdateNodeDescription(r.Get(4),"R5 (Neymar)");
    
    // Set positions to match your diagram: S left, C center, R1-R5 around C
    anim.SetConstantPosition(s.Get(0), 10, 50); // S (Sender) on left
    anim.SetConstantPosition(c.Get(0), 50, 50); // C (Classifier) in center
    anim.SetConstantPosition(r.Get(0), 80, 20); // R1 (Mbappe) top right
    anim.SetConstantPosition(r.Get(1), 85, 35); // R2 (Haaland) upper right
    anim.SetConstantPosition(r.Get(2), 85, 50); // R3 (Messi) center right
    anim.SetConstantPosition(r.Get(3), 85, 65); // R4 (Ronaldo) lower right
    anim.SetConstantPosition(r.Get(4), 80, 80); // R5 (Neymar) bottom right
    
    Simulator::Stop(Seconds(10)); Simulator::Run(); Simulator::Destroy(); return 0;
}
