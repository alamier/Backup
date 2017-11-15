//
// Created by Zhou Liu on 11/13/17.
//

#ifndef PROJECT3_PORTTABLE_H
#define PROJECT3_PORTTABLE_H


#include "global.h"
#define MAX_PORT_TTL 15

struct Port {
    unsigned short port_id;
    unsigned short router_id;
    unsigned int rtt;
    short TTL;
};

class PortTable {
public:
    PortTable();
    void init(unsigned short port_num, unsigned short router_id);
    unsigned short size();
    void increaseTTL();
    bool isChanged(queue<unsigned short> &expiredPorts);
    bool port2Id(unsigned short port, unsigned short& Id);
    bool Id2port(unsigned short Id, unsigned short& port);
    void refreshTTL(unsigned short port);
    bool getCostByPort(unsigned short port, unsigned int &cost);
private:
    Port* ports;
    unsigned short portNum;
    unsigned short routerId;
};


#endif //PROJECT3_PORTTABLE_H
