#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    // Command line parameters
    uint32_t nNodes = 1000;  // Number of nodes
    CommandLine cmd;
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.Parse(argc, argv);

    // Create nodes
    NodeContainer nodes;
    nodes.Create(nNodes);

    // Install CSMA
    CsmaHelper csma;
    NetDeviceContainer devices = csma.Install(nodes);

    // Install internet stack on nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Create UDP server application
    uint16_t serverPort = 9;
    UdpServerHelper server(serverPort);
    uint32_t serverNodeIndex = 257; // Jika <100 node maka index 3 Jika >255 node maka index 259
    ApplicationContainer serverApp = server.Install(nodes.Get(serverNodeIndex));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(10.0));

    // Create UDP client applications
    UdpClientHelper client(interfaces.GetAddress(serverNodeIndex), serverPort);
    client.SetAttribute("MaxPackets", UintegerValue(1));
    client.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client.SetAttribute("PacketSize", UintegerValue(1024));

    for (uint32_t i = 1; i < nNodes; ++i) {
        ApplicationContainer clientApp = client.Install(nodes.Get(i));
        clientApp.Start(Seconds(1.0));
        clientApp.Stop(Seconds(10.0));
    }

    // Enable pcap tracing
    csma.EnablePcap("csma/udp_multiple", devices.Get(serverNodeIndex), false);

    // Run simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
