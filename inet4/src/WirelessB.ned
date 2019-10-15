import inet.node.inet.INetworkNode;

network WirelessB extends WirelessA
{
    submodules:
        hostR1: <default("WirelessHost")> like INetworkNode {
            @display("p=250,300");
        }
        hostR2: <default("WirelessHost")> like INetworkNode {
            @display("p=150,450");
        }
        hostR3: <default("WirelessHost")> like INetworkNode {
            @display("p=350,450");
        }
}
