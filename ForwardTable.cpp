//
// Created by Zhou Liu on 11/13/17.
//

#include "ForwardTable.h"

void ForwardTable::init(unsigned short routerId, eProtocolType protocolType) {
  this->routerId = routerId;
  this->protocolType = protocolType;
  this->seqNum = 0;
}