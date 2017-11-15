#include "RoutingProtocolImpl.h"
#include "Node.h"
#include <limits.h>
#include <netinet/in.h>

RoutingProtocolImpl::RoutingProtocolImpl(Node *n) : RoutingProtocol(n) {
  sys = n;
  // add your own code
}

RoutingProtocolImpl::~RoutingProtocolImpl() {
  // add your own code (if needed)
}

void RoutingProtocolImpl::init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type) {
  // add your own code
  this->routerId = router_id;
  this->protocolType = protocol_type;

  portTable.init(num_ports, router_id);
  forwardTable.init(router_id, protocol_type);

  eAlarmType *alarmType = (eAlarmType *) malloc (sizeof(eAlarmType));
  *alarmType = ALARM_PING;
  sys->set_alarm(this, 0, alarmType);

  alarmType = (eAlarmType *) malloc(sizeof(eAlarmType));
  *alarmType = ALARM_ONE_SEC;
  sys->set_alarm(this, 1 * 1000, alarmType);

  alarmType = (eAlarmType *) malloc(sizeof(eAlarmType));
  *alarmType = protocol_type == P_DV ? ALARM_DV : ALARM_LS;
  sys->set_alarm(this, 30 * 1000, alarmType);
}

void RoutingProtocolImpl::handle_alarm(void *data) {
  // add your own code
  switch((eAlarmType)(*(eAlarmType*) data)){
    case ALARM_PING:
    {
      sys->set_alarm(this, 10 * 1000, data);
      for(unsigned short i = 0; i < portTable.size(); i++){
        send_pkt_ping(i);
      }
      break;
    }

    case ALARM_ONE_SEC:
    {
      sys->set_alarm(this, 1 * 1000, data);
      forwardTable.increaseTTL();
      bool forwardTableChanged = false;
      if(forwardTable.isChanged()){
        forwardTableChanged = true;
        if(protocolType == P_LS){
          unsigned short* newShortestPathTree = forwardTable.regenerateShortestPath();
          forwardTable.updateForwardTableAccordingToShortestPath(newShortestPathTree);
        }
      }
      portTable.increaseTTL();
      queue<unsigned short> expiredPorts;
      if(portTable.isChanged(expiredPorts)){
        while(!expiredPorts.empty()){
          unsigned short expiredID = expiredPorts.front();
          if(protocolType = P_DV){
            if(forwardTable.updateForwardTableDV(expiredID,expiredID,USHRT_MAX,USHRT_MAX)){
              forwardTableChanged = true;
            }
          }else{
            forwardTable.updateForwardTableLS(expiredID, USHRT_MAX);
            forwardTableChanged = true;
          }
          expiredPorts.pop();
        }
      }

      if(forwardTableChanged){
        if(protocolType == P_DV){
          unsigned short Id;
          for(int i = 0; i < portTable.size(); i++){
            if(!portTable.port2Id(i, Id)) continue;
            send_pkt_DV(Id);
          }
        }else{
          unsigned short* newShortestPathTree = forwardTable.regenerateShortestPath();
          forwardTable.updateForwardTableAccordingToShortestPath(newShortestPathTree);
          for(int i = 0; i < portTable.size(); i++) {
            send_pkt_LS(i);
            forwardTable.increaseSeqNum();
          }
        }
      }
      break;
    }

    case ALARM_DV:
    case ALARM_LS:{
      sys->set_alarm(this, 30 * 1000, data);
      if(protocolType == P_DV){
        unsigned short Id;
        for(int i = 0; i < portTable.size(); i++){
          if(!portTable.port2Id(i, Id)) continue;
          send_pkt_DV(Id);
        }
      }else{
        unsigned short Id;
        for(int i = 0; i < portTable.size(); i++){
          if(!portTable.port2Id(i, Id)) continue;
          send_pkt_DV(Id);
        }
      }
      break;
    }
  }
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
  // add your own code
  unsigned short packet_type = *((unsigned char *)packet);
  switch (packet_type) {
    case 1:{
      //PING packet
      char* pong_pkt =(char*)make_pkt_pong(port, packet);
      sys->send(port, pong_pkt, size);
      break;
    }
    case 2:{

      break;
    }
    case 3:{
      // DV packet
      unsigned int newCost;
      if(portTable.getCostByPort(port, newCost)){
      //TODO
      }
      break;
    }
    case 4:{

      break;
    }
    case 0:{

      break;
    }
  }
}

void RoutingProtocolImpl::send_pkt_ping(unsigned short portID) {
  unsigned short packet_size = sizeof(unsigned short) * 4 + sizeof(unsigned int);
  unsigned short* packet = (unsigned short*)malloc(packet_size);
  *(unsigned char *)packet = 1;
  *((unsigned short *)packet+1)=(unsigned short) htons((unsigned short)packet_size);
  *((unsigned short *)packet+2)=(unsigned short) htons((unsigned short)routerId);
  *((unsigned int *)packet+2)=(unsigned int) htonl((unsigned int)sys->time());

  sys->send(portID, packet, packet_size);
}

void RoutingProtocolImpl::send_pkt_DV(unsigned short destID) {
  unsigned short packet_size=8+4*forwardTable.size();
  short* packet=(short*)malloc(packet_size);

  *(unsigned char *)packet=3; //sPacketType[] = {"DATA","PING","PONG","DV","LS"};
  //write total size
  *((unsigned short *)packet+1)=(unsigned short) htons((unsigned short)packet_size);
  //write source id
  *((unsigned short *)packet+2)=(unsigned short) htons((unsigned short)routerId);
  //write dest id
  *((unsigned short *)packet+3)=(unsigned short) htons((unsigned short)destID);
  //write node
  int i=0;
  std::unordered_map<unsigned short, vector<ForwardEntry> >::iterator it;
  for(it =forwardTable.getForwardTable().begin(); it!=forwardTable.getForwardTable().end(); it++,i+=2){
    if((*it).second.empty())
      continue;
    *((unsigned short *)packet+4+i)=(unsigned short) htons((unsigned short)(*it).second.at(0).destID);
    if((*it).second.at(0).nextHop!=destID){
      *((unsigned short *)packet+5+i)=(unsigned short) htons((unsigned short)(*it).second.at(0).cost);
    }
    else{
      *((unsigned short *)packet+5+i)=(unsigned short) htons((unsigned short)USHRT_MAX);
    }
  }
  unsigned short port;
  portTable.Id2port(destID, port);
  sys->send(port, packet, packet_size);
}

void RoutingProtocolImpl::send_pkt_LS(unsigned short portID) {
  unsigned short packet_size = 12+4*forwardTable.getForwardTable()[routerId].size();
  char* packet = (char*)malloc(packet_size);
  if(forwardTable.getForwardTable().find(routerId) == forwardTable.getForwardTable().end()){
    std::cout<<"error [send_pkt_LS()]"<<std::endl;
  }
  //write packet type
  *(unsigned char *)packet=4; // 3 is the index of "LS" in sPacketType[] = {"DATA","PING","PONG","DV","LS"};
  //write total size
  *((unsigned short *)packet+1)=(unsigned short) htons((unsigned short)packet_size);
  //write source id
  *((unsigned short *)packet+2)=(unsigned short) htons((unsigned short)routerId);
  //write series number
  *((unsigned int *)packet+2)=(unsigned int) htonl((unsigned int)forwardTable.getSeqNum());

  for(unsigned int i=0;i<forwardTable.getForwardTable()[routerId].size();i++){
    //wirte neighbor node id
    *((unsigned short *)packet+6+2*i)=(unsigned short) htons((unsigned short)forwardTable.getForwardTable()[routerId].at(i).destID);
    //write cost
    *((unsigned short *)packet+7+2*i)=(unsigned short) htons((unsigned short)forwardTable.getForwardTable()[routerId].at(i).cost);
  }

  sys->send(portID, packet, packet_size);
}

void *RoutingProtocolImpl::make_pkt_pong(unsigned short port, void *ping_packet) {
  *((unsigned char*)ping_packet)=(char)2;      //{"DATA","PING","PONG","DV","LS"};
  unsigned short dest=(unsigned short) ntohs(*((unsigned short*)ping_packet+2));
  *((unsigned short *)ping_packet+3)=(unsigned short) htons((unsigned short)dest);
  *((unsigned short *)ping_packet+2)=(unsigned short) htons((unsigned short)routerId);
  portTable.refreshTTL(port);

  return ping_packet;
}

// add more of your own code
