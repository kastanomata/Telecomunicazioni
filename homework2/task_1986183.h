#include "ns3/core-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"

// CONSOLE MESSAGES
#define MATRICOLA_REF (std::string) "1986183"
#define TOO_MANY_ARGUMENTS "[Error] Troppi argomenti (max 3)."
#define MISSING_MATRICOLA "[Error] Inserire la matricola dello studente referente come parametro. (Hint: Gruppo 25)."
#define WRONG_MATRICOLA "[Error] La matricola inserita è errata. (Hint: Gruppo 25)."
#define RIGHT_MATRICOLA "\tMatricola inserita corretta. Proceeding..."
#define STOP_TIME 15.0          // spec: 15
#define N_ARGS 4                // spec: 4
#define MEGABYTES 1000000       // spec: 1000000
#define RTSCTS_STD_THRESHOLD 2347
#define GRUPPO_25_NAMES "Antonio,Turco,1986183,Alfredo,Segala,1999676,Aldo,Vitti,1986292,Alessandro,Temperini,1983516,Davide,Scolamiero,2022977,"

using namespace ns3;

std::string matricolaInserita = "";
bool enableRtsCts = false;
bool tracing = false;

// Controlla gli argomenti passati alla simulazione e setta le flag correttamente
void checkArgs(int argc, char * argv[]) {
    
    LogComponentEnable("Task_1986183", LOG_LEVEL_INFO);
    CommandLine cmd; // --print-help o --help per accedere a una descrizione delle flag da riga di comando
    cmd.AddValue("matricola-referente", "Matricola del referente del Gruppo 25", matricolaInserita);
    cmd.AddValue("enable-tracing", "Abilitare il tracing promiscuo dei pacchetti", tracing);
    cmd.AddValue("force-rts-cts", "Forzare l'uso di protocollo RTS/CTS su topologia wi-fi", enableRtsCts);
    cmd.Parse(argc, argv); 
    if(matricolaInserita.compare((std::string) "") == 0) {
        NS_LOG_ERROR(MISSING_MATRICOLA);
        exit(EXIT_FAILURE);
    } else if(matricolaInserita.compare(MATRICOLA_REF) != 0) {
        NS_LOG_ERROR(WRONG_MATRICOLA);
        exit(EXIT_FAILURE);
    } 
    // TODO: capire quali LogComponent vanno abilitate
    // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    // LogComponentEnable("BulkSendApplication", LOG_LEVEL_INFO);
    if(enableRtsCts) { 
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(0));
    } else { 
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(RTSCTS_STD_THRESHOLD));
    }
}

// Popola i NodeContainers con i nodi appropriati secondo le indicazioni progettuali, seguendo le indicazioni dell'immagine
void initializeNodes(NodeContainer *nodes, NodeContainer *p2pNodes, NodeContainer *wifiNodes, NodeContainer *csmaNodes) {
    nodes->Create(20); // Creo tutti i miei nodi 

    // Aggiungo i nodi collegati con CSMA nell mio Container specifico, mi aiuto con gli indici
    for(int i = 0; i < 3; i++)
        csmaNodes->Add(nodes->Get(i));

    // Aggiungo i nodi collegati in P2P nel mio Container specifico, mi aiuto con gli indici
    for(int i = 2; i < 11; i++)
        p2pNodes->Add(nodes->Get(i));
    
    // Aggiungo i nodi collegati in Wifi nel mio Container specifico, mi aiuto con gli indici
    for(int i = 10; i < 20; i++)
        wifiNodes->Add(nodes->Get(i));
}

// Controlla gli argomenti passati alla simulazione e setta le flag correttamente
void installP2PNetDevices(NodeContainer *nodes, NetDeviceContainer P2PDevices[], PointToPointHelper pointToPoint) {
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); 
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms")); 
    // DataRate = 5Mbps Delay = 20ms
    P2PDevices[0] = pointToPoint.Install(nodes->Get(5), nodes->Get(6));     // NetDevice P2P_0 -----> Node 5  Node 6
    P2PDevices[1] = pointToPoint.Install(nodes->Get(5), nodes->Get(7));     // NetDevice P2P_1 -----> Node 5  Node 7
    P2PDevices[2] = pointToPoint.Install(nodes->Get(6), nodes->Get(8));     // NetDevice P2P_2 -----> Node 6  Node 8
    P2PDevices[3] = pointToPoint.Install(nodes->Get(6), nodes->Get(9));     // NetDevice P2P_3 -----> Node 6  Node 9
    // DataRate = 100Mbps Delay = 20ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms"));  
    P2PDevices[4] = pointToPoint.Install(nodes->Get(2), nodes->Get(4));     // NetDevice P2P_4 -----> Node 2  Node 4
    P2PDevices[5] = pointToPoint.Install(nodes->Get(4), nodes->Get(5));     // NetDevice P2P_5 -----> Node 4  Node 5
    P2PDevices[6] = pointToPoint.Install(nodes->Get(4), nodes->Get(10));    // NetDevice P2P_6 -----> Node 4  Node 10
    // DataRate = 10Mbps Delay = 200ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("200ms"));
    P2PDevices[7] = pointToPoint.Install(nodes->Get(4), nodes->Get(3));     // NetDevice P2P_7 -----> Node 4  Node 3
}