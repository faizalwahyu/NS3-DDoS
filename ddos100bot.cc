#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  uint32_t nNodes = 100;

  NodeContainer nodes;
  nodes.Create (nNodes + 1);

  InternetStackHelper internet;
  internet.Install (nodes);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  NetDeviceContainer devices[nNodes];

  for(uint32_t i = 0; i < nNodes; ++i)
    {
      devices[i] = pointToPoint.Install (nodes.Get(i), nodes.Get(nNodes));

      Ipv4InterfaceContainer interfaces = address.Assign (devices[i]);
      address.NewNetwork ();
      
      BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (interfaces.GetAddress (1), 9));
      source.SetAttribute ("MaxBytes", UintegerValue (0));
      ApplicationContainer sourceApps = source.Install (nodes.Get (i));
      sourceApps.Start (Seconds (1.0));
      sourceApps.Stop (Seconds (10.0));
    }

  // Enable pcap tracing only on the target node
  pointToPoint.EnablePcap ("ddos_attack", devices[0].Get(1), true);

  // Setup mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (20),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // Enable animation
  AnimationInterface anim ("animation.xml");

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
