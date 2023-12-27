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
void initializeNodes(NodeContainer *nodes, NodeContainer *p2pNodes, NodeContainer *wifiNodes, NodeContainer *csmaNodes);

// Installa i netDevices sui nodi appropriati con topologia P2P
void installP2PNetDevices(NodeContainer *nodes, NetDeviceContainer P2PDevices[], uint16_t nP2PDevices, PointToPointHelper pointToPoint);

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

    initializeNodes(&nodes, &p2pNodes, &wifiStaNodes, &csmaNodes); 
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
    
    installP2PNetDevices(&nodes, P2PDevices, nP2PDevices, pointToPoint);  

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

    InternetStackHelper stack; // installazione indirizzi IP e network layer
    stack.Install(nodes);
    Ipv4AddressHelper address;
    // TODO: minimizzare gli indirizzi IP in modo che la mask sia più stringente possibile
    address.SetBase("10.1.1.0", "255.255.255.0"); // I dispositivi collegati alla rete CSMA appartengono alla sottorete 10.1.1.0 (mask: 255.255.255.0)
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    Ipv4InterfaceContainer P2PInterfaces[nP2PDevices];      // I dispositivi collegati ai P2P appartengono alle sottoreti da 10.1.2.0 a 10.1.9.0 (mask: 255.255.255.0)
    for(uint16_t i = 2; i < nP2PDevices + 2; i++) {         // Nei dispositivi P2P c'è il maggiore potenziale per diminuire la sottorete, visto che  
        std::stringstream networkStringStream;              // una sottorete potrebbe essere rappresentata con 2 bit (mask /32)
        networkStringStream<<"10.1."<<i<<".0";              // mi rendo conto che questa gestione delle stringhe è orribile ma sono le 2 di notte e C++ può andare a infilarsi le classi std::stringstream e std::string nel buco del culo
        const std::string tmp = networkStringStream.str();  // se non lo fai così per qualche motivo si incarta, quindi TODO: fix ma attenzione a testare per bene
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

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3)); // installazione del server sul nodo 3
    serverApps.Start(Seconds(1.0)); 
    serverApps.Stop(Seconds(10.0)); 

    UdpEchoClientHelper echoClient(P2PInterfaces[7].GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250)); 
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(20))); 
    echoClient.SetAttribute("PacketSize", UintegerValue(2032));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(7)); // installazione del client sul nodo 7
    clientApps.Start(Seconds(2.0)); 
    clientApps.Stop(Seconds(10.0)); 

    echoClient.SetFill(clientApps.Get(0), "Hello World"); // TODO: mettiamo nomi 
    
    // TODO: TCP
    // TCP N:1 delivery of a file of 1173 MB starting at 0.27 s
    // Sender: Node 17 Receiver: Server 0
    uint16_t receiverPort = 1; // porta a caso
    Address hubLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), receiverPort)); 
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", hubLocalAddress);
    ApplicationContainer receiver = packetSinkHelper.Install(nodes.Get(0)); // installo il packetSink sul nodo destinatario
    receiver.Start(Seconds(0.27));
    receiver.Stop(Seconds(20.0));

    OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address()); // un indirizzo a caso, tanto l'onOff non deve ricevere nulla
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.27]")); // da quando inizia a trasmettere i dati l'OnOff, da rivedere
    //onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); // da quando smette di trasmettere i dati l'OnOff, da rivedere

    uint16_t senderPort =  1; // porta a caso pt2 il ritorno
    ApplicationContainer sender;
    AddressValue remoteAddress(InetSocketAddress(csmaInterfaces.GetAddress(0), senderPort)); // indirizzo del nodo 0, si trova nelle CSMA interfaces
    onOffHelper.SetAttribute("Remote", remoteAddress); // assegna l'indirizzo del server all'Helper in modo che i pacchetti siano inviati al giusto destinatario
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1024)); // size del singolo frammento (forse MSS?)
    onOffHelper.SetAttribute("MaxBytes", UintegerValue((1173*1000000) / 8)); // quantità di dati da trasmettere
    sender.Add(onOffHelper.Install(nodes.Get(17))); // installo l'OnOff sul nodo mittente
    sender.Start(Seconds(0.27));
    sender.Stop(Seconds(20.0));

    // TCP N:2 delivery of a file of 1201 MB starting at 3.55 s
    // Sender: Node 9 Receiver: Server 1
    uint16_t receiverPort1 =2; // porta a caso
    Address hubLocalAddress1(InetSocketAddress(Ipv4Address::GetAny(), receiverPort1)); 
    PacketSinkHelper packetSinkHelper1("ns3::TcpSocketFactory", hubLocalAddress1);
    ApplicationContainer receiver1 = packetSinkHelper1.Install(nodes.Get(1)); // installo il packetSink sul nodo destinatario
    receiver1.Start(Seconds(3.55));
    receiver1.Stop(Seconds(20.0));

    OnOffHelper onOffHelper1("ns3::TcpSocketFactory", Address()); // un indirizzo a caso, tanto l'onOff non deve ricevere nulla
    onOffHelper1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=3.55]")); // da quando inizia a trasmettere i dati l'OnOff, da rivedere
    //onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); // da quando smette di trasmettere i dati l'OnOff, da rivedere

    uint16_t senderPort1 = 2; // porta a caso pt2 il ritorno
    ApplicationContainer sender1;
    AddressValue remoteAddress1(InetSocketAddress(csmaInterfaces.GetAddress(1), senderPort1)); // indirizzo del nodo 1, si trova nelle CSMA interfaces
    onOffHelper1.SetAttribute("Remote", remoteAddress1); // assegna l'indirizzo del server all'Helper in modo che i pacchetti siano inviati al giusto destinatario
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1024)); // size del singolo frammento (forse MSS?)
    onOffHelper1.SetAttribute("MaxBytes", UintegerValue((1201*1000000) / 8)); // quantità di dati da trasmettere
    sender1.Add(onOffHelper1.Install(nodes.Get(9))); // installo l'OnOff sul nodo mittente
    sender1.Start(Seconds(3.55));
    sender1.Stop(Seconds(20.0));

    // TCP N:3 delivery of a file of 1837 MB starting at 3.40 s
    // Sender: Node 14 Receiver: Server 0
    uint16_t receiverPort2 = 3; // porta a caso
    Address hubLocalAddress2(InetSocketAddress(Ipv4Address::GetAny(), receiverPort2)); 
    PacketSinkHelper packetSinkHelper2("ns3::TcpSocketFactory", hubLocalAddress2);
    ApplicationContainer receiver2 = packetSinkHelper2.Install(nodes.Get(0)); // installo il packetSink sul nodo destinatario
    receiver2.Start(Seconds(3.40));
    receiver2.Stop(Seconds(20.0));

    OnOffHelper onOffHelper2("ns3::TcpSocketFactory", Address()); // un indirizzo a caso, tanto l'onOff non deve ricevere nulla
    onOffHelper2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=3.40]")); // da quando inizia a trasmettere i dati l'OnOff, da rivedere
    //onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); // da quando smette di trasmettere i dati l'OnOff, da rivedere

    uint16_t senderPort2 = 3; // porta a caso pt2 il ritorno
    ApplicationContainer sender2;
    AddressValue remoteAddress2(InetSocketAddress(csmaInterfaces.GetAddress(0), senderPort2)); // indirizzo del nodo 0, si trova nelle CSMA interfaces
    onOffHelper2.SetAttribute("Remote", remoteAddress2); // assegna l'indirizzo del server all'Helper in modo che i pacchetti siano inviati al giusto destinatario
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1024)); // size del singolo frammento (forse MSS?)
    onOffHelper2.SetAttribute("MaxBytes", UintegerValue((1873*1000000) / 8)); // quantità di dati da trasmettere
    sender2.Add(onOffHelper2.Install(nodes.Get(14))); // installo l'OnOff sul nodo mittente
    sender2.Start(Seconds(3.40));
    sender2.Stop(Seconds(20.0));
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    if (tracing == true){ // tracing promiscuo per routers e switches (2, 4, 5, 10)
        csma.EnablePcap("task-2-1.pcap", csmaDevices.Get(2), true, true); // nodo 2 -> netDevice csma
        pointToPoint.EnablePcap("task", P2PDevices[1].Get(0), true);      // nodo 2 -> netDevice p2p 5--7
        pointToPoint.EnablePcap("task", P2PDevices[4].Get(1), true);      // nodo 4 -> netDevice p2p 4--2
        pointToPoint.EnablePcap("task", P2PDevices[7].Get(0), true);      // nodo 4 -> netDevice p2p 4--3
        pointToPoint.EnablePcap("task", P2PDevices[5].Get(0), true);      // nodo 4 -> netDevice p2p 4--5
        pointToPoint.EnablePcap("task", P2PDevices[6].Get(0), true);      // nodo 4 -> netDevice p2p 4--10
        pointToPoint.EnablePcap("task", P2PDevices[5].Get(1), true);      // nodo 5 -> netDevice p2p 5--4
        pointToPoint.EnablePcap("task", P2PDevices[0].Get(0), true);      // nodo 5 -> netDevice p2p 5--6
        pointToPoint.EnablePcap("task", P2PDevices[1].Get(0), true);      // nodo 5 -> netDevice p2p 5--7
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcap("task-10", adhocDevices.Get(0), true);             // nodo 10 -> netDevice wifi 

    } else { // tracing non promiscuo per routers e switches (2, 4, 5, 10)
        csma.EnablePcap("task-2-1.pcap", csmaDevices.Get(2), false, true);  // nodo 2 -> netDevice csma
        pointToPoint.EnablePcap("task", P2PDevices[1].Get(0));              // nodo 2 -> netDevice p2p 5--7
        pointToPoint.EnablePcap("task", P2PDevices[4].Get(1));              // nodo 4 -> netDevice p2p 4--2
        pointToPoint.EnablePcap("task", P2PDevices[5].Get(0));              // nodo 4 -> netDevice p2p 4--3
        pointToPoint.EnablePcap("task", P2PDevices[6].Get(0));              // nodo 4 -> netDevice p2p 4--5
        pointToPoint.EnablePcap("task", P2PDevices[7].Get(0));              // nodo 4 -> netDevice p2p 4--10
        pointToPoint.EnablePcap("task", P2PDevices[5].Get(1));              // nodo 5 -> netDevice p2p 5--4
        pointToPoint.EnablePcap("task", P2PDevices[0].Get(0));              // nodo 5 -> netDevice p2p 5--6
        pointToPoint.EnablePcap("task", P2PDevices[1].Get(0));              // nodo 5 -> netDevice p2p 5--7
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcap("task-10", adhocDevices.Get(0));                     // nodo 10 -> netDevice wifi
    }

    Simulator::Run();
    Simulator::Stop(Seconds(20));
    Simulator::Destroy();
    return 0;
}

void checkArgs(int argc, char * argv[]) {
    if(argc > N_ARGS) {
        std::cerr << TOO_MANY_ARGUMENTS;
        exit(EXIT_FAILURE);
    }
    std::string matricolaInserita = "";
    CommandLine cmd; // --print-help o --help per accedere a una descrizione delle flag da riga di comando
    cmd.AddValue("matricola-referente", "Matricola del referente del Gruppo 25", matricolaInserita);
    cmd.AddValue("enable-tracing", "Abilitare il tracing promiscuo dei pacchetti", tracing);
    cmd.AddValue("force-rts-cts", "Forzare l'uso di protocollo RTS/CTS", enableRtsCts);
    cmd.AddValue("verbose", "Set log level a LOG_LEVEL_INFO", verbose);
    cmd.Parse(argc, argv); 
    if(matricolaInserita.compare((std::string) "") == 0) {
        std::cerr << MISSING_MATRICOLA;
        exit(EXIT_FAILURE);
    } else if(matricolaInserita.compare(MATRICOLA_REF) != 0) {
        std::cerr << WRONG_MATRICOLA;
        exit(EXIT_FAILURE);
    } else {
        std::cout << RIGHT_MATRICOLA;
    }
    if(verbose) { // TODO: capire quali LogComponent vanno abilitate
        // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
        LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
        LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    } 
    if(enableRtsCts) { 
        std::cout << RTS_CTS_OK;
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(1));
    } else {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(100));
    }
    if(tracing) std::cout << TRACING_OK;
}

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

void installP2PNetDevices(NodeContainer *nodes, NetDeviceContainer P2PDevices[], uint16_t nP2PDevices, PointToPointHelper pointToPoint) {
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); 
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms")); 
    // DataRate = 5Mbps Delay = 20ms
    P2PDevices[0] = pointToPoint.Install(nodes->Get(5), nodes->Get(6)); // NetDevice P2P_0 contains 2 elements-----> Node 5  Node 6
    P2PDevices[1] = pointToPoint.Install(nodes->Get(5), nodes->Get(7)); // NetDevice P2P_1 contains 2 elements-----> Node 5  Node 7
    P2PDevices[2] = pointToPoint.Install(nodes->Get(6), nodes->Get(8)); // NetDevice P2P_2 contains 2 elements-----> Node 6  Node 8
    P2PDevices[3] = pointToPoint.Install(nodes->Get(6), nodes->Get(9)); // NetDevice P2P_3 contains 2 elements-----> Node 6  Node 9
    // DataRate = 100Mbps Delay = 20ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms"));  
    P2PDevices[4] = pointToPoint.Install(nodes->Get(2), nodes->Get(4)); // NetDevice P2P_4 contains 2 elements-----> Node 2  Node 4
    P2PDevices[5] = pointToPoint.Install(nodes->Get(4), nodes->Get(5)); // NetDevice P2P_5 contains 2 elements-----> Node 4  Node 5
    P2PDevices[6] = pointToPoint.Install(nodes->Get(4), nodes->Get(10)); // NetDevice P2P_6 contains 2 elements-----> Node 4  Node 10
    // DataRate = 10Mbps Delay = 200ms
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("200ms"));
    P2PDevices[7] = pointToPoint.Install(nodes->Get(4), nodes->Get(3));  // NetDevice P2P_7 contains 2 elements-----> Node 4  Node 3
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