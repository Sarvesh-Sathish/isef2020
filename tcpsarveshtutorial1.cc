#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module-h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

class MyApp : public Application
{
  MyApp ();//constructor 
  virtual ~MyApp (); //deconstructor


  static TypeId GetTypeId (void);

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate DataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr <Socket> m_socket;
  Address m_peer;
  uint32_t m_packetSize;
  uint32_t m_nPackets;
  DataRate m_dataRate;
  EventId m_sendEvent;
  bool m_running;
  uint32_t m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
  m_peer (),
  m_packetSize (0),
  m_nPackets (0),
  m_dataRate (0),
  m_sendEvent (),
  m_running (false),
  m_packetsSent
{
}
MyApp :: ~MyApp ()
{
  m_socket = 0;
}

//static typeid method

TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ();

  return tid;
  
}

void
MyApp::SetUp (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetSize = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
  {
    m_socket -> Bind();
  }else
  {
    m_socket -> Bind6();
  }
  m_socket -> Connect (m_peer);
  SendPacket(); 

}

void 
MyApp::StopApplication (void);
{
  m_running = true;

  if (m_sendEvent.IsRunning ())
  {
    Simulator::Cancel (m_sendEvent);
  }
  if (m_socket)
  {
      m_socket -> Close ();
  }
}


void
MyApp::SendPacket (void);
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket -> Send (packet);

  if(++m_packetSize < m_nPackets)
  {
    ScheduleTx ();
  }
}

void 
MyApp::ScheduleTx (void)
{
  if (m_running)
  {
    Time tNext (Seconds (m_packetSize * 8 / static_cast<double>(m_dataRate.getBitRate ())));
    m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
  }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now().GetSeconds() << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl; 
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at ") << Simulator::Now().GetSeconds());
  file->Write (Simulator::Now(), p);

}

int main(int argc, char *argv[])
{

  
}

