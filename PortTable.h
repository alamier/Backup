//
// Created by Zhou Liu on 11/13/17.
//

#ifndef PROJECT3_PORTTABLE_H
#define PROJECT3_PORTTABLE_H


#include "global.h"

struct Port {
    unsigned short port_id;
    unsigned short router_id;
    unsigned int rtt;
    short TTL;
};

class PortTable {
public:
    PortTable(){};
    void init(unsigned short port_num, unsigned short router_id);

private:
    Port* ports;
    unsigned short portNum;
    unsigned short routerId;
};


#endif //PROJECT3_PORTTABLE_H
