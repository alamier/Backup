//
// Created by Zhou Liu on 11/13/17.
//

#ifndef PROJECT3_FORWARDTABLE_H
#define PROJECT3_FORWARDTABLE_H

#include "global.h"
#include <limits.h>
#define MAX_DV_LS_TTL 45

struct ForwardEntry{
    ForwardEntry(){};
    ForwardEntry(unsigned short destID, unsigned short cost, unsigned short nextHop,
                 unsigned int seqNum):destID(destID), cost(cost), nextHop(nextHop), seqNum(seqNum){};
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
    bool updateForwardTableDV(unsigned short destID, unsigned short nextHop, unsigned short newCost, unsigned short oldCost);
    bool updateForwardTableLS(unsigned short destID, unsigned short newCost);
    int size();
    void increaseSeqNum();

private:
    unsigned short routerId;
    eProtocolType protocolType;
    unsigned int seqNum;
public:
    unsigned int getSeqNum() const;

private:
    std::unordered_map<unsigned short, vector<ForwardEntry>> forwardTable;
public:
    unordered_map<unsigned short, vector<ForwardEntry>> getForwardTable();
};


#endif //PROJECT3_FORWARDTABLE_H
