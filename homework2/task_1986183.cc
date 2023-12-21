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

using namespace ns3;

bool enableRtsCts = false;
bool tracing = false;
bool verbose = true;

NS_LOG_COMPONENT_DEFINE("Task_1986183");

// Controlla gli argomenti passati alla simulazione e setta le flag correttamente
void checkArgs(int argc, char * argv[]);

// Popola i NodeContainers con i nodi secondo le indicazioni progettuali
void createAllNodes(NodeContainer *nodes, NodeContainer *p2pNodes, NodeContainer *wifiNodes, NodeContainer *csmaNodes);

int main(int argc, char* argv[]) {
    checkArgs(argc, argv); 

    // Come scelta progettuale abbiamo deciso di creare:
    NodeContainer nodes;            // un Container generale per una gestione migliore degli indici [0-19];
    NodeContainer p2pNodes;         // un Container per nodi collegati da topologie P2P;
    NodeContainer wifiStaNodes;     // un Container per i nodi collegati via topologia Wi-fi;
    NodeContainer csmaNodes;        // un Container per i nodi collegati con protocollo CSMA.

    createAllNodes(&nodes, &p2pNodes, &wifiStaNodes, &csmaNodes);

    // Creo un helper  per i  nodi CSMA
    CsmaHelper csma; // DataRate = 10Mbps Delay = 200ms
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));
    // Creo la netdevice per i CSMA
    NetDeviceContainer csmaDevices = csma.Install(csmaNodes);

    // Creo 3 per helper per i nodi P2P
    PointToPointHelper pointToPoint5;       // DataRate = 5Mbps Delay = 20ms
    pointToPoint5.SetDeviceAttribute("DataRate", StringValue("5Mbps")); 
    pointToPoint5.SetChannelAttribute("Delay", StringValue("20ms")); 
    PointToPointHelper pointToPoint100;     // DataRate = 100Mbps Delay = 20ms
    pointToPoint100.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint100.SetChannelAttribute("Delay", StringValue("20ms"));
    PointToPointHelper pointToPoint10;      // DataRate = 10Mbps Delay = 200ms
    pointToPoint10.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint10.SetChannelAttribute("Delay", StringValue("200ms"));

    // Creo i netdevices per i P2P 
    // DataRate = 5Mbps Delay = 20ms
    NetDeviceContainer devices_5_7 = pointToPoint5.Install(nodes.Get(5), nodes.Get(7));
    NetDeviceContainer devices_5_6 = pointToPoint5.Install(nodes.Get(5), nodes.Get(6));
    NetDeviceContainer devices_6_8 = pointToPoint5.Install(nodes.Get(6), nodes.Get(8));
    NetDeviceContainer devices_6_9 = pointToPoint5.Install(nodes.Get(6), nodes.Get(9));
    // DataRate = 100Mbps Delay = 20ms
    NetDeviceContainer devices_2_4 = pointToPoint100.Install(nodes.Get(2), nodes.Get(4));
    NetDeviceContainer devices_4_5 = pointToPoint100.Install(nodes.Get(4), nodes.Get(5));
    NetDeviceContainer devices_4_10 = pointToPoint100.Install(nodes.Get(4), nodes.Get(10));
    // DataRate = 10Mbps Delay = 200ms
    NetDeviceContainer devices_4_3 = pointToPoint10.Install(nodes.Get(4), nodes.Get(3));

    // WifI
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    NetDeviceContainer adhocDevices;
    mac.SetType("ns3::AdhocWifiMac");
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

    exit(EXIT_SUCCESS); // FINO A QUI FUNZIONA, DA SOTTO È DA RIVEDERE

    InternetStackHelper stack;
    stack.Install(nodes);

    NetDeviceContainer p2pDevicesMacro;
    p2pDevicesMacro.Add(devices_5_6);
    p2pDevicesMacro.Add(devices_6_8);
    p2pDevicesMacro.Add(devices_6_9);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.248"); // TODO: check IP addresses
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    address.SetBase("10.1.2.0", "255.255.255.240"); // TODO: check IP addresses
    Ipv4InterfaceContainer wifiInterfaces;
    wifiInterfaces = address.Assign(adhocDevices);

    address.SetBase("10.1.3.0", "255.255.255.248"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterfacesMacro;
    p2pInterfacesMacro = address.Assign(p2pDevicesMacro);

    address.SetBase("10.1.4.0", "255.255.255.252"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterface_5_7;
    p2pInterface_5_7 = address.Assign(devices_5_7);

    address.SetBase("10.1.4.0", "255.255.255.252", "0.0.0.3"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterface_4_5;
    p2pInterface_4_5 = address.Assign(devices_4_5);

    address.SetBase("10.1.4.0", "255.255.255.252", "0.0.0.5"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterface_2_4;
    p2pInterface_2_4 = address.Assign(devices_2_4);

    address.SetBase("10.1.4.0", "255.255.255.252", "0.0.0.7"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterface_4_3;
    p2pInterface_4_3 = address.Assign(devices_4_3);

    address.SetBase("10.1.4.0", "255.255.255.252", "0.0.0.10"); // TODO: check IP addresses
    Ipv4InterfaceContainer p2pInterface_4_10;
    p2pInterface_4_10 = address.Assign(devices_4_10);

    //Manca invio pacchetti TCP

    // UDP Echo application with Client 7 and Server 3
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(15.0));

    UdpEchoClientHelper echoClient(p2pInterface_5_7.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250)); // MaxPackets: 250
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(20.0))); // Bytes Periodicity:20ms
    echoClient.SetAttribute("PacketSize", UintegerValue(2032)); //Size of packet: 2032

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(7));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(15.0));

    // Aggiungere matricole
    echoClient.SetFill(clientApps.Get(0), "Antonio, Turco, 1986183, Alfredo, Segala, 1999676, Aldo Vitti, , Alessandro, Temperini, , Davide, Scolamiero, ");

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Abilitare tracing per nodi router (2, 4, 5, 10)
    if (tracing == true){
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcap("Router_10", adhocDevices.Get(0), true);
        pointToPoint100.EnablePcap("Router_2", devices_2_4.Get(0), true);
        pointToPoint100.EnablePcap("Router_4", devices_2_4.Get(1), true);
        pointToPoint100.EnablePcap("Router_5", devices_4_5.Get(1), true);
    } else {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcapAll("Wi_Fi");
        pointToPoint100.EnablePcapAll("P2P_100");
        pointToPoint10.EnablePcapAll("P2P_10");
        pointToPoint5.EnablePcapAll("P2P_5");
        csma.EnablePcapAll("CSMA");
    }

    Simulator::Run();
    Simulator::Stop(Seconds(15));
    Simulator::Destroy();
    return 0;
}

void checkArgs(int argc, char * argv[]) {
    if(argc > 5) {
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
    if(!verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
        return;
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
