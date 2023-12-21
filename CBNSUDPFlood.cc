#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    // Command line parameters
    uint32_t nNodes;  
    // Jumlah perangkat yang terlibat di atur nNodes mulai dari 1000,2500,5000, 7500, 10.000
    uint32_t serverNodeIndex;
    CommandLine cmd;
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("serverNodeIndex", "Target IP", serverNodeIndex);
    cmd.Parse(argc, argv);
    uint32_t nNodesfix=nNodes+2;

    // Membuat jumlah node
    NodeContainer nodes;
    nodes.Create(nNodesfix);

    // Install protokol CSMA
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate("10Mbps")));
    NetDeviceContainer devices = csma.Install(nodes);

    // Install internet stack on nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Create UDP server application
    uint16_t serverPort = 80;
    UdpServerHelper server(serverPort);
    //IP 10.0.1.7 = 262 (Web Server)
    //IP 10.0.1.6 = 261 (Nginx)
    //IP 10.0.1.4 = 259 (HAProxy)
    //uint32_t serverNodeIndex = 262; // Jika <100 node maka index 3 Jika >255 node maka index 259
    ApplicationContainer serverApp = server.Install(nodes.Get(serverNodeIndex));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(10.0));

    // Create UDP client applications
    UdpClientHelper client(interfaces.GetAddress(serverNodeIndex), serverPort);
    client.SetAttribute("MaxPackets", UintegerValue(1000));
    client.SetAttribute("Interval", TimeValue(Seconds(1)));
    client.SetAttribute("PacketSize", UintegerValue(1024));

    for (uint32_t i = 1; i < nNodesfix; ++i) {
        ApplicationContainer clientApp = client.Install(nodes.Get(i));
        clientApp.Start(Seconds(1.0));
        clientApp.Stop(Seconds(240.0));
    }
    
    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0), "MinY", DoubleValue(0.0), "DeltaX", DoubleValue(5.0), "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(5), "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    mobility.Install(nodes);
    
    // Mengonversi nilai numerik menjadi string
    std::string nNodesString = std::to_string(nNodes);

    // Menggabungkan string "CBNS/udpflood" dengan nilai variabel nNodesString
    //std::string animationname = "CBNS/udpflood" + nNodesString + "Nodes.xml";
    
    //   AnimationInterface anim(animationname);
    //anim.EnablePacketMetadata("csma/udp_multiple", true, false);
    //ns3::AnimationInterface::SetConstantPosition(nodes.Get(0), 0, 0);
    ns3::AnimationInterface::SetConstantPosition(nodes.Get(serverNodeIndex), 0, 20, 0);
    //ns3::AnimationInterface::SetConstantPosition(nodes.Get(2), 20, 10);

    // Enable pcap tracing
    std::string pcapFileName = "CBNS/udpflood" + nNodesString + "Nodes";
    csma.EnablePcap(pcapFileName, devices.Get(serverNodeIndex), false);

    // Run simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
