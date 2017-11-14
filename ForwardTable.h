//
// Created by Zhou Liu on 11/13/17.
//

#ifndef PROJECT3_FORWARDTABLE_H
#define PROJECT3_FORWARDTABLE_H

#include "global.h"
#define MAX_DV_LS_TTL 45

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
    void increaseTTL();
    bool isChanged();
    unsigned short* regenerateShortestPath();
    bool updateForwardTableAccordingToShortestPath(unsigned short* path_to);

private:
    unsigned short routerId;
    eProtocolType protocolType;
    unsigned int seqNum;
    std::unordered_map<unsigned short, vector<ForwardEntry>> forwardTable;
};


#endif //PROJECT3_FORWARDTABLE_H
