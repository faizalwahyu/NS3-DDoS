#include <iostream>
#include <fstream>
#include <cstdlib> // For rand()
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CsmaDDoSExample");

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (101);  // 10 nodes and 1 central node (hub)

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  NetDeviceContainer devices = csma.Install (nodes);

  InternetStackHelper internet;
  internet.Install (nodes);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;
  Ipv4Address targetIp("10.0.1.2"); // Specify the target IP address

  for (uint32_t i = 1; i <= 50; ++i) {
    // Generate a random source IP address for each attacking node
    Ipv4Address sourceIp(("10.0.1." + std::to_string(rand() % 100 + 2)).c_str());

    OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (targetIp, port)));
    onoff.SetConstantRate (DataRate ("2048kb/s")); // Increase the data rate

    ApplicationContainer app = onoff.Install (nodes.Get (i));

    // Set the source IP address for the OnOff application
    app.Get (0)->GetObject<OnOffApplication>()->SetAttribute ("Remote", AddressValue (InetSocketAddress (targetIp, port)));
    app.Get (0)->GetObject<OnOffApplication>()->SetAttribute ("Local", AddressValue (InetSocketAddress (sourceIp, 0))); 

    app.Start (Seconds (5.0)); // Start the attack at a later time
    app.Stop (Seconds (20.0)); // Extend the duration of the attack
  }

  NS_LOG_INFO ("Configure Tracing.");
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("PCAPs/csmaddos.tr"));
  csma.EnablePcapAll ("PCAPs/csmaddos2", false);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  return 0;
}
