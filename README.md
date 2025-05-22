# NS3-DDoS
Created to simulate a DDoS attack using NS3.
The result of the simulation is a pcap output file that will be used to analyze the performance of a Webserver using Load Balancer Nginx and HAProxy.

To run the file, first create a folder named CBNS. The pcap output will be saved there, and only one pcap file will be generated.
The file that runs the simulation is udpflood.cc.

To modify the number of nodes (nNodes), you can use the syntax:
./ns3 run "scratch/CBNSUDPFlood.cc --nNodes=500 --serverNodeIndex=262"

You can change the nNodes parameter to 500, 1000, 1500, or 2000.

For the serverNodeIndex parameter, the IPs and their corresponding indices are:

    IP 10.0.1.7 = 262 (Web Server) 

    IP 10.0.1.6 = 261 (Nginx) 

    IP 10.0.1.4 = 259 (HAProxy) 
