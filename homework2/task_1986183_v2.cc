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

// CONSOLE MESSAGES
#define MATRICOLA_REF (std::string) "1986183"
#define MISSING_MATRICOLA "[Error] Inserire la matricola dello studente referente come parametro. (Hint: Gruppo 25).\n"
#define WRONG_MATRICOLA "[Error] La matricola inserita è errata. (Hint: Gruppo 25).\n"
#define RIGHT_MATRICOLA "\tMatricola inserita corretta. Proceeding...\n"
#define RTS_CTS_OK "\tRTS/CTS abilitato. Proceeding...\n"
#define TRACING_OK "\tTracing promiscuo abilitato. Proceeding...\n"
#define TOO_MANY_ARGUMENTS "[Error] Troppi argomenti (max 3).\n"
#define N_ARGS 5
using namespace ns3;

bool enableRtsCts = false;
bool tracing = false;
bool verbose = true;


NS_LOG_COMPONENT_DEFINE("Task_1986183");

// Controlla gli argomenti passati alla simulazione e setta le flag correttamente
void checkArgs(int argc, char * argv[]);

// Popola i NodeContainers con i nodi appropriati secondo le indicazioni progettuali, seguendo le indicazioni dell'immagine
void createAllNodes(NodeContainer *nodes, NodeContainer *p2pNodes, NodeContainer *wifiNodes, NodeContainer *csmaNodes);

// Stampa informazioni per ogni Node (a quali topologie sia collegato)
void printAllNodes(NodeContainer nodes, NodeContainer p2pNodes, NodeContainer wifiNodes, NodeContainer csmaNodes);

// Stampa informazioni per ogni NetDevice (per ogni topologia stampa i nodi che vi partecipano) 
void printAllNetDevices(NetDeviceContainer csmaDevices, NetDeviceContainer devices[], uint16_t nP2PDevices, NetDeviceContainer adhocDevices);

// Stampa informazioni per ogni Interfaccia (gli indirizzi IP)
void printAllInterfaces(Ipv4InterfaceContainer interface, NetDeviceContainer container);

int main(int argc, char* argv[]) {
    checkArgs(argc, argv); 

    // Come scelta progettuale abbiamo deciso di creare:
    NodeContainer nodes;            // un Container generale per una gestione migliore degli indici [0-19];
    NodeContainer p2pNodes;         // un Container per nodi collegati da topologie P2P;
    NodeContainer wifiStaNodes;     // un Container per i nodi collegati via topologia Wi-fi;
    NodeContainer csmaNodes;        // un Container per i nodi collegati con protocollo CSMA.

    createAllNodes(&nodes, &p2pNodes, &wifiStaNodes, &csmaNodes); 
    if(verbose) printAllNodes(nodes, p2pNodes, wifiStaNodes, csmaNodes);

    // Creo un helper  per i  nodi CSMA
    CsmaHelper csma; // DataRate = 10Mbps Delay = 200ms
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));
    // Creo la netdevice per i CSMA
    NetDeviceContainer csmaDevices = csma.Install(csmaNodes);

    // Creo 3 per helper per i nodi P2P
    uint16_t nP2PDevices = 8; // abbiamo un numero di collegamenti p2p uguale a 8
    NetDeviceContainer P2PDevices[nP2PDevices]; // creo un array piuttosto che singoli NetDeviceContainer, comodo per passare a funzioni e fare loop 
    PointToPointHelper pointToPoint; // alla fine ho preferito usare un solo PointToPointHelper perché rende più facile settare il tracing
    // DataRate = 5Mbps Delay = 20ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); 
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms")); 
    P2PDevices[0] = pointToPoint.Install(nodes.Get(5), nodes.Get(6)); // NetDevice P2P_0 contains 2 elements-----> Node 5  Node 6
    P2PDevices[1] = pointToPoint.Install(nodes.Get(5), nodes.Get(7)); // NetDevice P2P_1 contains 2 elements-----> Node 5  Node 7
    P2PDevices[2] = pointToPoint.Install(nodes.Get(6), nodes.Get(8)); // NetDevice P2P_2 contains 2 elements-----> Node 6  Node 8
    P2PDevices[3] = pointToPoint.Install(nodes.Get(6), nodes.Get(9)); // NetDevice P2P_3 contains 2 elements-----> Node 6  Node 9
    // DataRate = 100Mbps Delay = 20ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms"));  
    P2PDevices[4] = pointToPoint.Install(nodes.Get(2), nodes.Get(4)); // NetDevice P2P_4 contains 2 elements-----> Node 2  Node 4
    P2PDevices[5] = pointToPoint.Install(nodes.Get(4), nodes.Get(5)); // NetDevice P2P_5 contains 2 elements-----> Node 4  Node 5
    P2PDevices[6] = pointToPoint.Install(nodes.Get(4), nodes.Get(10)); // NetDevice P2P_6 contains 2 elements-----> Node 4  Node 10
    // DataRate = 10Mbps Delay = 200ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("200ms"));
    P2PDevices[7] = pointToPoint.Install(nodes.Get(4), nodes.Get(3));  // NetDevice P2P_7 contains 2 elements-----> Node 4  Node 3  

    // Wifi -> tutta la parte del wi-fi è un miracolo di Natale se funziona, non mi azzarderei a toccarla
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211g);                // nuove specifiche del professore
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");   // nuove specifiche del professore

    WifiMacHelper mac;
    NetDeviceContainer adhocDevices;
    mac.SetType("ns3::AdhocWifiMac"); // in quanto modalità adHoc non abbiamo bisogno di settare SSID
    adhocDevices = wifi.Install(phy, mac, wifiStaNodes);

    // Mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(10.0),
                                  "MinY",
                                  DoubleValue(10.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(2.0),
                                  "GridWidth",
                                  UintegerValue(5),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);

    if(verbose) printAllNetDevices(csmaDevices, P2PDevices, nP2PDevices,  adhocDevices);

    InternetStackHelper stack; // istallazione indirizzi IP e network layer
    stack.Install(nodes);
    Ipv4AddressHelper address;
    // TODO: minimizzare gli indirizzi IP in modo che la mask sia più stringente possibile
    address.SetBase("10.1.1.0", "255.255.255.0"); // I dispositivi collegati alla rete CSMA appartengono tutti alla sottorete 10.1.1.0 (mask: 255.255.255.0)
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    Ipv4InterfaceContainer P2PInterfaces[nP2PDevices];  // I dispositivi collegati ai P2P appartengono alle sottoreti da 10.1.2.0 a 10.1.9.0 (mask: 255.255.255.0)
    for(uint16_t i = 2; i < nP2PDevices + 2; i++) {     // Nei dispositivi P2P c'è il maggiore potenziale per diminuire la sottorete, visto che potenzialmente una sottorete
        std::stringstream networkStringStream;          // potrebbe essere rappresentata con 2 bit (mask /32)
        networkStringStream<<"10.1."<<i<<".0";          // mi rendo conto che questa gestione delle stringhe è orribile ma sono le 2 di notte e C++ può andare a infilarsi le classi std::stringstream e std::string nel buco del culo
        const std::string tmp = networkStringStream.str(); // se non lo fai così per qualche motivo si incarta, quindi TODO: fix ma attenzione a testare per bene
        const char *networkIp = tmp.c_str();
        address.SetBase(networkIp, "255.255.255.0");
        P2PInterfaces[i-2] = address.Assign(P2PDevices[i-2]);
    }

    address.SetBase("10.1.10.0", "255.255.255.0"); // I dispositivi collegati al wi-fi appartengono alla sottorete 10.1.10.0 (mask: 255.255.255.0)
    Ipv4InterfaceContainer wifiInterfaces;
    wifiInterfaces = address.Assign(adhocDevices);

    if(verbose) {                                           // la funzione printAllInterfaces è stata realizzata puntando alla  
        std::cout << "CSMA IPv4: \n";                       // modularità piuttosto che alla struttura del nostro scriptino
        printAllInterfaces(csmaInterfaces, csmaDevices);    // secondo me anche le altre andrebbero riscritte puntando a una struttura simile
        std::cout << "P2P IPv4: \n";
        for(uint16_t i = 0; i < nP2PDevices; i++) {
            printAllInterfaces(P2PInterfaces[i], P2PDevices[i]);        
        }
        std::cout << "WiFi IPv4: \n";
        printAllInterfaces(wifiInterfaces, adhocDevices);
    }

    // UDP Echo application with Client 7 and Server 3
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(7)); // istallazione del server sul nodo 7
    serverApps.Start(Seconds(1.0)); // TODO: rivedi questi valori che li ho copia incollati dall'esempio del Lacava
    serverApps.Stop(Seconds(10.0)); // TODO: rivedi questi valori che li ho copia incollati dall'esempio del Lacava

    UdpEchoClientHelper echoClient(P2PInterfaces[1].GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250)); 
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0))); // TODO: rivedi questi valori che li ho copia incollati dall'esempio del Lacava
    echoClient.SetAttribute("PacketSize", UintegerValue(2032));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(3));
    clientApps.Start(Seconds(2.0)); // TODO: rivedi questi valori che li ho copia incollati dall'esempio del Lacava
    clientApps.Stop(Seconds(10.0)); // TODO: rivedi questi valori che li ho copia incollati dall'esempio del Lacava

    pointToPoint.EnablePcap("Server[7]_", P2PDevices[1].Get(1) , true); // Gestione della generazione di file .pcap provvisoria
    pointToPoint.EnablePcap("Client[3]_", P2PDevices[7].Get(1), true);  // Poi la si metterà sotto e solo per i nodi router, qui volevo verificare UDP funzionasse (e pare di sì)
    
    //TODO: TCP
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    /*
    // TODO: Abilitare tracing per nodi router (2, 4, 5, 10)
    if (tracing == true){
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcap("Router_10", adhocDevices.Get(0), true);
        pointToPoint.EnablePcap("Router_2", devices_2_4.Get(0), true);
        pointToPoint.EnablePcap("Router_4", devices_2_4.Get(1), true);
        pointToPoint.EnablePcap("Router_5", devices_4_5.Get(1), true);
    } else {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcapAll("Wi_Fi");
        pointToPoint.EnablePcapAll("P2P");
        csma.EnablePcapAll("CSMA");
    }
    */
    Simulator::Run();
    Simulator::Stop(Seconds(15));
    Simulator::Destroy();
    return 0;
}

void checkArgs(int argc, char * argv[]) {
    if(argc > N_ARGS) {
        std::cerr << TOO_MANY_ARGUMENTS;
        exit(EXIT_FAILURE);
    }
    std::string matricolaInserita = "";
    CommandLine cmd;
    cmd.AddValue("matricola-referente", "Matricola del referente del Gruppo 25", matricolaInserita);
    cmd.AddValue("verbose", "Set log level a LOG_LEVEL_INFO", verbose);
    cmd.AddValue("enable-tracing", "Abilitare il tracing promiscuo dei pacchetti", tracing);
    cmd.AddValue("force-rts-cts", "Forzare l'uso di protocollo RTS/CTS", enableRtsCts);
    cmd.Parse (argc, argv); 
    if(matricolaInserita.compare((std::string) "") == 0) {
        std::cerr << MISSING_MATRICOLA;
        exit(EXIT_FAILURE);
    } else if(matricolaInserita.compare(MATRICOLA_REF) != 0) {
        std::cerr << WRONG_MATRICOLA;
        exit(EXIT_FAILURE);
    } else {
        std::cout << RIGHT_MATRICOLA;
    }
    if(verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    } 
    if(enableRtsCts) {
        std::cout << RTS_CTS_OK;
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(1));
    } else {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(100));
    }
    if(tracing) std::cout << TRACING_OK;  
     
}

void createAllNodes(NodeContainer *nodes, NodeContainer *p2pNodes, NodeContainer *wifiNodes, NodeContainer *csmaNodes) {
    nodes->Create(20); // Creo tutti i miei nodi 

    // Aggiungo i nodi collegati con CSMA nell mio Container specifico , mi aiuto con gli indici
    /*
    for(int i = 0; i < 2; i++)
        csmaNodes->Add(nodes->Get(i));
    */
    csmaNodes->Add(nodes->Get(0));
    csmaNodes->Add(nodes->Get(1));
    csmaNodes->Add(nodes->Get(2));

    // Aggiungo i nodi collegati in P2P nel mio Container specifico , mi aiuto con gli indici
    /*
    for(int i = 2; i < 11; i++)
        p2pNodes->Add(nodes->Get(i));
    */
    p2pNodes->Add(nodes->Get(2));
    p2pNodes->Add(nodes->Get(3));
    p2pNodes->Add(nodes->Get(4));
    p2pNodes->Add(nodes->Get(5));
    p2pNodes->Add(nodes->Get(6));
    p2pNodes->Add(nodes->Get(7));
    p2pNodes->Add(nodes->Get(8));
    p2pNodes->Add(nodes->Get(9));
    p2pNodes->Add(nodes->Get(10));

    // Aggiungo i nodi collegati in Wifi nel mio Container specifico , mi aiuto con gli indici
    /*
    for(int i = 10; i < 20; i++)
        p2pNodes->Add(nodes->Get(i));
    */
    wifiNodes->Add(nodes->Get(10));
    wifiNodes->Add(nodes->Get(11));
    wifiNodes->Add(nodes->Get(12));
    wifiNodes->Add(nodes->Get(13));
    wifiNodes->Add(nodes->Get(14));
    wifiNodes->Add(nodes->Get(15));
    wifiNodes->Add(nodes->Get(16));
    wifiNodes->Add(nodes->Get(17));
    wifiNodes->Add(nodes->Get(18));
    wifiNodes->Add(nodes->Get(19));
}

void printAllNodes(NodeContainer nodes, NodeContainer p2pNodes, NodeContainer wifiNodes, NodeContainer csmaNodes) {
    for(uint16_t i = 0; i < nodes.GetN(); i++){
        uint16_t id = nodes.Get(i)->GetId();
        std::cout << "Nodo indice: " << i << "\n";
        if(p2pNodes.Contains(id)) 
            std::cout << "\tE' estremo di almeno un collegamento p2p\n";
        if(csmaNodes.Contains(id))
            std::cout << "\tFa parte del circuito CSMA\n";
        if(wifiNodes.Contains(id))
            std::cout << "\tE' collegato alla rete wi-fi\n";
    }
}

void printAllNetDevices(NetDeviceContainer csmaDevices, NetDeviceContainer devices[], uint16_t nP2PDevices, NetDeviceContainer wifiDevices) {
    std:: cout << "NetDevices connected to CSMA: ";
    for(uint16_t i = 0; i< csmaDevices.GetN(); i++) {
        if(i != csmaDevices.GetN()-1)
            std::cout<< csmaDevices.Get(i)->GetNode()->GetId() << ", ";
        else
            std::cout << csmaDevices.Get(i)->GetNode()->GetId() << ".\n";
    }
    for(uint16_t i = 0; i < nP2PDevices; i++) {
        NetDeviceContainer p2pdev = devices[i];
        std::cout << "NetDevice P2P_" << i << " contains " << p2pdev.GetN() << " elements----->";
        // pointToPoint.SetDeviceAttribute("DataRate", StringValue(???)); 
        // pointToPoint.SetChannelAttribute("Delay", StringValue(???));
        // TODO: trovare un modo per ottenere dal NetDevice o dal Channel gli attributi usati dall'Helper per verificarli
        // ci sono stato una buona mezz'ora senza capire se ci sia un modo, sulla documentazione non ho trovato nulla
        Ptr<NetDevice> dev = p2pdev.Get(0); // <--- questa cosa è un residuo del mio tentativo, potrebbe portare a qualcosa?
        for(uint16_t j = 0; j < p2pdev.GetN(); j++)  {
            std::cout << "\tNode " << p2pdev.Get(j)->GetNode()->GetId();
        }
        std::cout<<"\n";
        
    }
    std:: cout << "NetDevices connected to Wi-fi: ";
    for(uint16_t i = 0; i < wifiDevices.GetN(); i++) {
        if(i != wifiDevices.GetN()-1)
            std::cout<< wifiDevices.Get(i)->GetNode()->GetId() << ", ";
        else 
            std::cout<< wifiDevices.Get(i)->GetNode()->GetId() << ".\n";
    }
}

void printAllInterfaces(Ipv4InterfaceContainer interface, NetDeviceContainer container) {
    for(uint16_t i = 0; i < container.GetN(); i++) {
        std::cout << "Interfaccia dell'" << i << "th netDevice: "<<interface.GetAddress(i);
        std::cout << ", Node n" << container.Get(i)->GetNode()->GetId() << "\n";
    }
}

//