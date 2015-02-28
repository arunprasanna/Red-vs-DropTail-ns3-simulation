//include necessary ns3 modules
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/uinteger.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/test.h"

using namespace ns3;

int main (int argc, char *argv[])
{
//std::cout << "\n Running Simulation " << std::endl;
Time::SetResolution (Time::NS);
std::string queueType = "DropTail";
//initialize all parameters used
int nodes = 0; 
int window_size = 4000; 
int maxBytes = 1000000;
int qlen = 800;
double minimum_threshold = 50.0;
double maximum_threshold = 150.0;
double Wq = 1.0/100.0;
double maxP = 1.0/10.0;
double load = 0.75;
//debug mode
#ifdef DEBUG
std::cout<< "\nDEBUG MODE ON" <<std::endl;
#endif

//get command line arguments if they've been given
CommandLine cmd;
cmd.AddValue ("window_size", "TCP Receiver window size", window_size);
cmd.AddValue ("maxBytes", "Max bytes that can be sent", maxBytes);
cmd.AddValue ("minTh", "Min threshold for prob drops", minimum_threshold);
cmd.AddValue ("maximum_threshold", "Max threshold for problitistic drops", maximum_threshold);
cmd.AddValue ("maxP", "Max probability of doing an early drop", maxP);
cmd.AddValue ("Wq", "Weighting factor for average queue length computation", Wq);
cmd.AddValue ("qlen", "Maximum number of bytes that can be enqueued", qlen);
cmd.AddValue ("load", "Load", load);
cmd.Parse(argc, argv);
#ifdef DEBUG
{
	std::cout << "\nInput values" << std::endl;
std::cerr << "queueType = " << queueType << "\n"<< "load=" << load << "\n"
<< "window_size=" << window_size << "\n"<< "maxBytes=" << maxBytes << "\n"
<< "\tminimum_threshold=" << minimum_threshold << "\n"<< "\tmaximum_threshold=" << maximum_threshold << "\n"
<< "\tmaxP=" << maxP << "\n"<< "\tWq=" << Wq << "\n"<< "\tqueue_length=" << qlen << "\n"<< std::endl;
}	
#endif
//set configuration for DropTailQueue, RedQUeue and TCPSocket
Config::SetDefault ("ns3::DropTailQueue::Mode", EnumValue(DropTailQueue::QUEUE_MODE_BYTES));
Config::SetDefault ("ns3::DropTailQueue::MaxBytes", UintegerValue(maxBytes));
Config::SetDefault ("ns3::RedQueue::Mode", EnumValue (RedQueue::QUEUE_MODE_BYTES));
Config::SetDefault ("ns3::RedQueue::MinTh", DoubleValue (minimum_threshold));
Config::SetDefault ("ns3::RedQueue::MaxTh", DoubleValue (maximum_threshold));
Config::SetDefault ("ns3::RedQueue::QW", DoubleValue (Wq));
Config::SetDefault ("ns3::RedQueue::QueueLimit", UintegerValue (qlen));
Config::SetDefault ("ns3::RedQueue::LInterm", DoubleValue (maxP));
Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue(window_size));


std::string type;//define a string
type = "ns3::RedQueue"; //change to DropTailQueue if needed

NodeContainer n;
//number of nodes we are going to create. Max is just 10 in our topology.
nodes=12;
n.Create(nodes);

//define topology by defining the links for nodes created
//change this below to obtain different topologies discussed in the report
NodeContainer bottleneck_link_1 = NodeContainer(n.Get(0), n.Get(1));
NodeContainer bottleneck_link_3 = NodeContainer(n.Get(1), n.Get(2));
NodeContainer bottleneck_link_2 = NodeContainer(n.Get(0), n.Get(3));
NodeContainer source_1 = NodeContainer(n.Get(3), n.Get(4));
NodeContainer source_2 = NodeContainer(n.Get(3), n.Get(5));
NodeContainer source_3 = NodeContainer(n.Get(3), n.Get(6));
NodeContainer sink_1 = NodeContainer(n.Get(2), n.Get(7));
NodeContainer sink_2 = NodeContainer(n.Get(2), n.Get(8));
NodeContainer sink_3 = NodeContainer(n.Get(2), n.Get(9));

//define the router nodes 
NodeContainer routerNodes;
routerNodes.Add(n.Get(0));
routerNodes.Add(n.Get(1));
routerNodes.Add(n.Get(2));
routerNodes.Add(n.Get(3));


//define the left nodes that act as source
NodeContainer leftNodes;
leftNodes.Add(n.Get(4));
leftNodes.Add(n.Get(5));
leftNodes.Add(n.Get(6));

//define the right nodes that act as sink
NodeContainer rightNodes;
rightNodes.Add(n.Get(7));
rightNodes.Add(n.Get(8));
rightNodes.Add(n.Get(9));

//call point to point helper to set bandwidth and speed of light delay and specify type of queue management!
PointToPointHelper bottleneck1;
bottleneck1.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
bottleneck1.SetChannelAttribute ("Delay", StringValue ("10ms"));
bottleneck1.SetQueue (type);
NetDeviceContainer device_bottleneck_link_2 = bottleneck1.Install(bottleneck_link_2);

PointToPointHelper bottleneck2;
bottleneck2.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
bottleneck2.SetChannelAttribute ("Delay", StringValue ("10ms"));
bottleneck2.SetQueue (type);
NetDeviceContainer device_bottleneck_link_3 = bottleneck2.Install(bottleneck_link_3);

PointToPointHelper center;
center.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
center.SetChannelAttribute ("Delay", StringValue ("10ms"));
NetDeviceContainer device_bottleneck_link_1 = center.Install(bottleneck_link_1);

PointToPointHelper p2pLeft;
p2pLeft.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
p2pLeft.SetChannelAttribute ("Delay", StringValue ("6ms"));
NetDeviceContainer device_source_1 = p2pLeft.Install(source_1);

PointToPointHelper p2pLeft2;
p2pLeft2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
p2pLeft2.SetChannelAttribute ("Delay", StringValue ("6ms"));
NetDeviceContainer device_source_2 = p2pLeft2.Install(source_2);
//give the other TCP connection more bandwidth to get varied results
PointToPointHelper p2pLeft3;
p2pLeft3.SetDeviceAttribute ("DataRate", StringValue ("16Mbps"));
p2pLeft3.SetChannelAttribute ("Delay", StringValue ("4ms"));
NetDeviceContainer device_source_3 = p2pLeft3.Install(source_3);

PointToPointHelper p2pRight;
p2pRight.SetDeviceAttribute ("DataRate", StringValue ("16Mbps"));
p2pRight.SetChannelAttribute ("Delay", StringValue ("4ms"));
NetDeviceContainer device_sink_1 = p2pRight.Install(sink_1);

PointToPointHelper p2pRight2;
p2pRight2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
p2pRight2.SetChannelAttribute ("Delay", StringValue ("6ms"));
NetDeviceContainer device_sink_2 = p2pRight2.Install(sink_2);

PointToPointHelper p2pRight3;
p2pRight3.SetDeviceAttribute ("DataRate", StringValue ("16Mbps"));
p2pRight3.SetChannelAttribute ("Delay", StringValue ("4ms"));
NetDeviceContainer device_sink_3 = p2pRight3.Install(sink_3);

NetDeviceContainer array_of_devices[] = {device_bottleneck_link_2, device_bottleneck_link_3, device_bottleneck_link_1, device_source_1, device_source_2, device_source_3, device_sink_1, device_sink_2, device_sink_3};
std::vector<NetDeviceContainer> devices(array_of_devices, array_of_devices + sizeof(array_of_devices) / sizeof(NetDeviceContainer));
//install the internet stack using the internetstackhelper
InternetStackHelper stack;
stack.Install (n);

//create the IP addresses required and give it the same subnet mask as required
Ipv4AddressHelper ipv4;
std::vector<Ipv4InterfaceContainer> ifaceLinks(nodes-1);
for(uint32_t i=0; i<devices.size(); ++i)
 {
std::ostringstream sub_net;
sub_net << "192.10." << i+1 << ".0";
ipv4.SetBase(sub_net.str().c_str(), "255.255.255.0");
ifaceLinks[i] = ipv4.Assign(devices[i]);
}
//define min bandwidth
double min_bandwidth = 1000000;
// define duty cycle and determine rate of on-off application
double duty_cycle = 0.5;
uint64_t rate = (uint64_t)(load*min_bandwidth / 3 / duty_cycle); 

//define a random port
int port_number = 12;
 //set UDP to on off application and determine it using the rate calculated above
OnOffHelper onOffUdp1("ns3::UdpSocketFactory", Address(InetSocketAddress(ifaceLinks[6].GetAddress(1), port_number)));
onOffUdp1.SetConstantRate(DataRate(rate));
onOffUdp1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
onOffUdp1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));


//I have used Bulksendhelper for TCP connections. UDP connection load suffices to provide us trends on a congested network.
BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress(ifaceLinks[7].GetAddress(1), port_number));
source1.SetAttribute("MaxBytes", UintegerValue (maxBytes));

BulkSendHelper source2 ("ns3::TcpSocketFactory", InetSocketAddress(ifaceLinks[8].GetAddress(1), port_number));
source2.SetAttribute("MaxBytes", UintegerValue (maxBytes));

ApplicationContainer sourceApps;
//define the sources and start them
sourceApps.Add(source1.Install(source_2.Get(1)));
sourceApps.Add(source2.Install(source_3.Get(1)));
sourceApps.Add(onOffUdp1.Install(source_1.Get(1)));
sourceApps.Start(Seconds(0.0));
sourceApps.Stop(Seconds(15.0));
//define the sinks
PacketSinkHelper sinkUdp1("ns3::UdpSocketFactory",
Address(InetSocketAddress(Ipv4Address::GetAny(), port_number)));

PacketSinkHelper sinkTcp1("ns3::TcpSocketFactory",
Address(InetSocketAddress(Ipv4Address::GetAny(), port_number)));

PacketSinkHelper sinkTcp2("ns3::TcpSocketFactory",
Address(InetSocketAddress(Ipv4Address::GetAny(), port_number)));

ApplicationContainer sinkApps;
sinkApps.Add(sinkTcp1.Install(sink_2.Get(1)));
sinkApps.Add(sinkTcp2.Install(sink_3.Get(1)));
sinkApps.Add(sinkUdp1.Install(sink_1.Get(1)));
//sink has same start and end time as source
sinkApps.Start(Seconds(0.0));
sinkApps.Stop(Seconds(15.0));

//dont forget to populate the routing tables!
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

Simulator::Stop (Seconds (15.0));
Simulator::Run ();

std::vector<int> goodputs;
int i = 0;
//Iterate through the flows
for(ApplicationContainer::Iterator ii = sinkApps.Begin(); ii != sinkApps.End(); ++ii) {
Ptr<PacketSink> sink = DynamicCast<PacketSink> (*ii);
int bytes_received = sink->GetTotalRx ();//get total bytes received
goodputs.push_back(bytes_received / 15.0);//total bytes divided by time
if(i==0)
{
std::cout << "\nTCP1 :" << std::endl;
std::cout << "\tTotal Bytes Received: " << bytes_received << std::endl;
std::cout << "\tGoodput: " << goodputs.back() << " Bytes/seconds" << std::endl;
}
if(i==1)
{
std::cout << "\nTCP2 :" << std::endl;
std::cout << "\tTotal Bytes Received: " << bytes_received << std::endl;
std::cout << "\tGoodput: " << goodputs.back() << " Bytes/seconds" << std::endl;
}
if(i==2)
{
std::cout << "\nUDP:" << std::endl;
std::cout << "\tTotal Bytes Received: " << bytes_received << std::endl;
std::cout << "\tGoodput: " << goodputs.back() << " Bytes/seconds" << std::endl;
}
++i;
}

std::cout << "\nIt worked!" << std::endl;
std::cerr << "queueType = " << queueType << "\n"<< "load=" << load << "\n"
<< "window_size=" << window_size << "\n"<< "maxBytes=" << maxBytes << "\n"
<< "\tminimum_threshold=" << minimum_threshold << "\n"<< "\tmaximum_threshold=" << maximum_threshold << "\n"
<< "\tmaxP=" << maxP << "\n"<< "\tWq=" << Wq << "\n"<< "\tqueue_length=" << qlen << "\n"<< std::endl;

Simulator::Destroy ();

return 0;
}
