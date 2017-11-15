//
// Created by Zhou Liu on 11/13/17.
//

#include "PortTable.h"

PortTable::PortTable() {}

void PortTable::init(unsigned short port_num, unsigned short router_id) {
  this->portNum = port_num;
  this->ports = (Port*)malloc(sizeof(Port) * port_num);
  for(int i = 0; i < port_num; i++){
    ports[i].TTL = -1;
  }
  this->routerId = router_id;
}

unsigned short PortTable::size() {
  return this->portNum;
}

void PortTable::increaseTTL() {
  for(int i = 0; i < portNum; i++) {
    if(ports[i].TTL < 0) continue;
    ports[i].TTL ++;
  }
}

bool PortTable::isChanged(queue<unsigned short> &expiredPorts) {
  bool changed = false;
  for (int i = 0; i < portNum; i++){
    if(ports[i].TTL < 0) continue;
    if(ports[i].TTL > MAX_PORT_TTL){
      expiredPorts.push(ports[i].router_id);
      ports[i].TTL = -1;
      changed = true;
    }
  }
  return changed;
}

bool PortTable::port2Id(unsigned short port, unsigned short &Id) {
  if(port > portNum - 1) {
    std::cout<<"Error: [port2Id()] port id exceeds max num"<<std::endl;
    return false;
  }
  if(ports[port].TTL < 0) {
    return false;
  }
  else{
    Id = ports[port].router_id;
    return true;
  }
}

bool PortTable::Id2port(unsigned short Id, unsigned short &port) {
  for(int i=0;i<portNum;i++){
    if(ports[i].TTL<0)
      continue;
    if(ports[i].router_id==Id){
      port=ports[i].port_id;
      return true;
    }
  }
  return false;
}

void PortTable::refreshTTL(unsigned short port) {
  if(port>portNum-1){
    std::cout<<"Error [refreshTTL(unsigned short port)]"<<std::endl;
    return;
  }
  if(ports[port].TTL<0){
    return;
  }
  else{
    ports[port].TTL=0;
  }
}

bool PortTable::getCostByPort(unsigned short port, unsigned int &cost) {
  if(ports[port].TTL<0) return false;
  cost=ports[port].rtt;
  return true;
}
