#include "task_1986183.h"

int main(int argc, char* argv[]) {
    checkArgs(argc, argv); 
    // Come scelta progettuale abbiamo deciso di creare:
    NodeContainer nodes;            // un Container generale per una gestione migliore degli indici [0-19];
    NodeContainer p2pNodes;         // un Container per nodi collegati da topologie P2P;
    NodeContainer wifiStaNodes;     // un Container per i nodi collegati via topologia Wi-fi;
    NodeContainer csmaNodes;        // un Container per i nodi collegati con protocollo CSMA.

    initializeNodes(&nodes, &p2pNodes, &wifiStaNodes, &csmaNodes);

    CsmaHelper csma; // Creo un helper per i nodi CSMA, con Attributes DataRate = 10Mbps e Delay = 200ms
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));
    NetDeviceContainer csmaDevices = csma.Install(csmaNodes); // Installo i NetDevice sui nodi appropriati

    PointToPointHelper pointToPoint;                // Creo un helper per i nodi Point2Point, con Attributes DataRate e Delay settati di volta in volta secondo le specifiche
    uint16_t nP2PDevices = 8;                       // abbiamo un numero di collegamenti p2p uguale a 8
    NetDeviceContainer P2PDevices[nP2PDevices];     // creo un array piuttosto che singoli NetDeviceContainer, comodo per passare a funzioni e fare loop 
    installP2PNetDevices(&nodes, P2PDevices, nP2PDevices, pointToPoint); 

    // Wifi 
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);                 // Secondo indicazioni
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");   // Secondo indicazioni

    WifiMacHelper mac;
    NetDeviceContainer adhocDevices;
    mac.SetType("ns3::AdhocWifiMac"); // in quanto modalità adHoc non abbiamo bisogno di settare SSID
    adhocDevices = wifi.Install(phy, mac, wifiStaNodes);

    // Mobility
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);

    InternetStackHelper stack; // installazione indirizzi IP e network layer
    stack.Install(nodes);
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer csmaInterfaces;
    Ipv4InterfaceContainer wifiInterfaces;
    uint16_t nP2PInterfaces = 8;
    Ipv4InterfaceContainer P2PInterfaces[8];

    address.SetBase("10.1.1.128", "255.255.255.248");
    csmaInterfaces = address.Assign(csmaDevices);
    address.SetBase("10.1.1.192", "255.255.255.240");
    wifiInterfaces = address.Assign(adhocDevices);
    char buf[16];
    uint8_t p2pPartialSubnetAddress = 224;
    for(uint8_t i = 0; i < nP2PInterfaces; i++) {
        sprintf(buf, "10.1.1.%d", p2pPartialSubnetAddress+i*4);
        address.SetBase(buf, "255.255.255.252");
        P2PInterfaces[i] = address.Assign(P2PDevices[i]);
    }

    // UDP Echo application 
    // Receiver(UdpEchoServerApplication):   Server 3
    // Sender(UdpEchoClientApplication):     Node 17
    UdpEchoServerHelper echoServer(DISCARD_PORT);
    Address echoServerAddress = P2PInterfaces[7].GetAddress(1);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3)); // installazione del server sul nodo 3
    serverApps.Start(Seconds(0.0)); 
    serverApps.Stop(Seconds(STOP_TIME)); 

    UdpEchoClientHelper echoClient(echoServerAddress, DISCARD_PORT);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250)); 
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(20))); 
    echoClient.SetAttribute("PacketSize", UintegerValue(2032*8));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(17)); // installazione del client sul nodo 7
    echoClient.SetFill(clientApps.Get(0), "Antonio,Turco,1986183,Alfredo,Segala,1999676,Aldo,Vitti,1986292,Alessandro,Temperini,1983516,Davide,Scolamiero,2022977"); 
    clientApps.Start(Seconds(0.0)); 
    clientApps.Stop(Seconds(STOP_TIME)); 

    // TCP N:1 delivery of a file of 1173 MB starting at 0.27s
    // Receiver(PacketSink):            Server 0
    // Sender(BulkSendApplication):     Node 17 
    uint16_t port = 2239;
    PacketSinkHelper sink_n1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps_n1 = sink_n1.Install(nodes.Get(0));
    sinkApps_n1.Start(Seconds(0.0));
    sinkApps_n1.Stop(Seconds(STOP_TIME));

    Ipv4Address sinkAddress_n1 = csmaInterfaces.GetAddress(0);
    BulkSendHelper source_n1("ns3::TcpSocketFactory", InetSocketAddress(sinkAddress_n1, port));
    uint32_t maxBytes_n1 = 1173 * MEGABYTES;
    source_n1.SetAttribute("MaxBytes", UintegerValue(maxBytes_n1));
    ApplicationContainer sourceApps_n1 = source_n1.Install(nodes.Get(17));
    sourceApps_n1.Start(Seconds(0.27));
    sourceApps_n1.Stop(Seconds(STOP_TIME));

    // TCP N:2 delivery of a file of 1201 MB starting at 3.55s
    // Receiver(PacketSink):            Server 1
    // Sender(BulkSendApplication):     Node 9 
    port = 4543;
    PacketSinkHelper sink_n2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps_n2 = sink_n2.Install(nodes.Get(1)); // sink_n2.Install(nodes.Get(3)); // 
    sinkApps_n2.Start(Seconds(0.0));
    sinkApps_n2.Stop(Seconds(STOP_TIME));

    Ipv4Address sinkAddress_n2 = csmaInterfaces.GetAddress(1); // P2PInterfaces[7].GetAddress(1); // 
    BulkSendHelper source_n2("ns3::TcpSocketFactory", InetSocketAddress(sinkAddress_n2, port));
    uint32_t maxBytes_n2 = 1201 * MEGABYTES;
    source_n2.SetAttribute("MaxBytes", UintegerValue(maxBytes_n2));
    ApplicationContainer sourceApps_n2 = source_n2.Install(nodes.Get(9));
    sourceApps_n2.Start(Seconds(3.55));
    sourceApps_n2.Stop(Seconds(STOP_TIME));
    
    // TCP N:3 delivery of a file of 1837 MB starting at 3.40s
    // Receiver(PacketSink):            Server 0
    // Sender(BulkSendApplication):     Node 14
    port = 7378;
    PacketSinkHelper sink_n3("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps_n3 = sink_n3.Install(nodes.Get(0));
    sinkApps_n3.Start(Seconds(0.0));
    sinkApps_n3.Stop(Seconds(STOP_TIME));

    Ipv4Address sinkAddress_n3 = csmaInterfaces.GetAddress(0);
    BulkSendHelper source_n3("ns3::TcpSocketFactory", InetSocketAddress(sinkAddress_n3, port));
    uint32_t maxBytes_n3 = 1837 * MEGABYTES;
    source_n3.SetAttribute("MaxBytes", UintegerValue(maxBytes_n3));
    ApplicationContainer sourceApps_n3 = source_n3.Install(nodes.Get(14));
    sourceApps_n3.Start(Seconds(3.40));
    sourceApps_n3.Stop(Seconds(STOP_TIME));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NodeContainer routers;
    routers.Add(nodes.Get(2));
    routers.Add(nodes.Get(4));
    routers.Add(nodes.Get(5));
    routers.Add(nodes.Get(10));
    
    if(tracing) {
      pointToPoint.EnablePcap("task", routers, true);
      csma.EnablePcap("task", routers, true);
      phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
      phy.EnablePcap("task", routers, true);
    }
    

    Simulator::Stop(Seconds(STOP_TIME));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
//