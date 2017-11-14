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
  portTable.init(num_ports, router_id);
  forwardTable.init(router_id, protocol_type);

  eAlarmType *alarmType = (eAlarmType *) malloc (sizeof(eAlarmType));
  *alarmType = ALARM_PING;
  sys->set_alarm(this, 0, alarmType);
}

void RoutingProtocolImpl::handle_alarm(void *data) {
  // add your own code
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
  // add your own code
}

// add more of your own code
