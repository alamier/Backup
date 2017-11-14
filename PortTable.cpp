//
// Created by Zhou Liu on 11/13/17.
//

#include "PortTable.h"

PortTable::PortTable() {}

void PortTable::init(unsigned short port_num, unsigned short router_id) {
  this->portNum = port_num;
  this->routerId = router_id;
}