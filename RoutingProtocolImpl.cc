#include "RoutingProtocolImpl.h"
#include "Node.h"

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
      sys->set_alarm(this, 10 * 1000, data);
      for(unsigned short i = 0; i < portTable.size(); i++){
        send_pkt_ping(i);
      }
      break;
    case ALARM_ONE_SEC:
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

      break;
    case ALARM_DV:
    case ALARM_LS:

      break;
  }
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
  // add your own code
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

// add more of your own code
