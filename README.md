# NS3-DDoS
Dibuat untuk melakukan simulasi serangan DDoS dengan NS3
Hasil dari simulasi tersebut berupa output pcap yang akan digunakan untuk analisis performa Webserver dengan menggunakan Load Balancer Nginx dan HAProxy

Untuk running file, bisa create folder dengan nama CBNS terlebih dahulu, output pcap akan disana, dan hanya akan muncul 1 file pcap

Untuk modifikasi nNodes bisa menggunakan sintax ./ns3 run "scratch/CBNSUDPFlood.cc --nNodes=1000 --serverNodeIndex=262""
Bisa diganti parameter nNodes menjadi 2500, 5000, 7500, dan 10.000
Untuk parameter serverNodeIndex = 
IP 10.0.1.7 = 262 (Web Server) Faizal
IP 10.0.1.6 = 261 (Nginx) Azza
IP 10.0.1.4 = 259 (HAProxy) Wira
