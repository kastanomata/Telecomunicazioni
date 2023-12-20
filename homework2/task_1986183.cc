#include <stdlib.h>
#include <cstring>
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

// Network Topology

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1986183");

int main(int argc, char* argv[]){

    char* studentId = (char *) malloc(strlen(argv[1]));
    studentId = argv[1];
    
    switch(argc) {
        case 1:
            std::cerr << "Inserire matricola studente come parametro d'esecuzione referente (Hint: Antonio Turco)\n";
            exit(EXIT_FAILURE);
            break;
        case 2:
            std::cout << "Matricola inserita: " << studentId << "\n";
            if(strncmp(studentId, "1986183", strlen(studentId))!=0){
                std::cerr << "Matricola Errata\n";
                exit(EXIT_FAILURE);
            }
            break;
        default: 
            std::cerr << "Troppi argomenti inseriti (max 1)\n";
            exit(EXIT_FAILURE);
    }
    char c;

    bool enableRtsCts = false;
    bool tracing = false;

    std::cout << "Forzare RtS/CtS nei pacchetti? [Y/n]\n";
    std::cin >> c;
    if(c=='Y' || c == 'y'){
        enableRtsCts = true;
    }
    
    std::cout << "Abilitare tracing promiscuo? [Y/n]\n";
    std::cin >> c;
    if(c=='Y' || c == 'y'){
        tracing = true;
    }


    std::cout << "Inizio Simulazione\n"; 
    if(enableRtsCts) std::cout << "Rts/CtS abilitato\n";
    if(tracing) std::cout << "tracing promiscuo abilitato\n"; 

    //Come scelta progettuale abbiamo deciso di creare un Container generale per una gestione migliore degli indici [0-19]
    // e in secondo luogo abbiamno deciso di creare dei Container specifici per nodi collegati in P2P un Container per i nodi in wifi 
    // e un Container per i nodi in CSMA

    NodeContainer nodes; // Container per tutti i nodi
    NodeContainer p2pNodes; // Container per i nodi in P2P
    NodeContainer wifiNodes; // Container per i nodi in Wifi
    NodeContainer csmaNodes; // Container per i nodi in CSMA

    nodes.Create(20); // Creo tutti i miei nodi 

    // Aggiungo i nodi collegati con CSMA nell mio Container specifico , mi aiuto con gli indici
    csmaNodes.Add(nodes.Get(0));
    csmaNodes.Add(nodes.Get(1));
    csmaNodes.Add(nodes.Get(2));

    // Aggiungo i nodi collegati in P2P nel mio Container specifico , mi aiuto con gli indici
    p2pNodes.Add(nodes.Get(2));
    p2pNodes.Add(nodes.Get(3));
    p2pNodes.Add(nodes.Get(4));
    p2pNodes.Add(nodes.Get(5));
    p2pNodes.Add(nodes.Get(6));
    p2pNodes.Add(nodes.Get(7));
    p2pNodes.Add(nodes.Get(8));
    p2pNodes.Add(nodes.Get(9));
    p2pNodes.Add(nodes.Get(10));

    // Aggiungo i nodi collegati in Wifi nel mio Container specifico , mi aiuto con gli indici
    wifiNodes.Add(nodes.Get(10));
    wifiNodes.Add(nodes.Get(11));
    wifiNodes.Add(nodes.Get(12));
    wifiNodes.Add(nodes.Get(13));
    wifiNodes.Add(nodes.Get(14));
    wifiNodes.Add(nodes.Get(15));
    wifiNodes.Add(nodes.Get(16));
    wifiNodes.Add(nodes.Get(17));
    wifiNodes.Add(nodes.Get(18));
    wifiNodes.Add(nodes.Get(19));

    //Creo un helper  per i  nodi CSMA
    CsmaHelper csma1;
    CsmaHelper csma2;

    csma1.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma1.SetChannelAttribute("Delay", StringValue("200ms"));

    csma2.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma2.SetChannelAttribute("Delay", StringValue("200ms"));

    //Creo 3 per helper per i nodi P2P
    PointToPointHelper pointToPoint5;
    pointToPoint5.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint5.SetChannelAttribute("Delay", StringValue("20ms"));

    PointToPointHelper pointToPoint100;
    pointToPoint100.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint100.SetChannelAttribute("Delay", StringValue("20ms"));

    PointToPointHelper pointToPoint10;
    pointToPoint10.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint10.SetChannelAttribute("Delay", StringValue("200ms"));

    //TODO Antonio :
        //Creo un helper per nodi Wifi





    //

    // Creo la netdevice per i CSMA
    NetDeviceContainer devices_0_2 = csma1.Install(nodes.Get(0));
    NetDeviceContainer devices_2_0 = csma1.Install(nodes.Get(2));
    NetDeviceContainer devices_1_2 = csma2.Install(nodes.Get(1));
    NetDeviceContainer devices_2_1 = csma2.Install(nodes.Get(2));


    //Creo i netdevices per i P2P
    NetDeviceContainer devices_2_4 = pointToPoint100.Install(nodes.Get(2) , nodes.Get(4));
    NetDeviceContainer devices_4_5 = pointToPoint100.Install(nodes.Get(4) , nodes.Get(5));
    NetDeviceContainer devices_4_10 = pointToPoint100.Install(nodes.Get(4) , nodes.Get(10));
    //
    NetDeviceContainer devices_4_3 = pointToPoint10.Install(nodes.Get(4) , nodes.Get(3));
    //
    NetDeviceContainer devices_5_7 = pointToPoint5.Install(nodes.Get(5) , nodes.Get(7));
    NetDeviceContainer devices_5_6 = pointToPoint5.Install(nodes.Get(5) , nodes.Get(6));
    NetDeviceContainer devices_6_8 = pointToPoint5.Install(nodes.Get(6) , nodes.Get(8));
    NetDeviceContainer devices_6_9 = pointToPoint5.Install(nodes.Get(6) , nodes.Get(9));
    
    //LogComponentEnable("UdpEchoClientApplication", LOG_FUNCTION);
    //LogComponentEnable("UdpEchoServerApplication", LOG_FUNCTION);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}