//
// Created by Zhou Liu on 11/13/17.
//

#ifndef PROJECT3_FORWARDTABLE_H
#define PROJECT3_FORWARDTABLE_H

#include "global.h"

struct ForwardEntry{
    unsigned short destID;
    unsigned short cost;
    unsigned short nextHop;
    short TTL;
    unsigned int seqNum;
};

class ForwardTable {
public:
    ForwardTable();
    void init(unsigned short routerId, eProtocolType protocolType);

private:
    unsigned short routerId;
    eProtocolType protocolType;
    unsigned int seqNum;
};


#endif //PROJECT3_FORWARDTABLE_H
