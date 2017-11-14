//
// Created by Zhou Liu on 11/13/17.
//

#include "ForwardTable.h"

ForwardTable::ForwardTable() {}

void ForwardTable::init(unsigned short routerId, eProtocolType protocolType) {
  this->routerId = routerId;
  this->protocolType = protocolType;
  this->seqNum = 0;
}

void ForwardTable::increaseTTL() {
  std::unordered_map<unsigned short, vector<ForwardEntry>>::iterator it;
  for(it = forwardTable.begin(); it != forwardTable.end(); it++) {
    if(!(*it).second.empty()){
      (*it).second.at(0).TTL ++;
    }
  }
}

bool ForwardTable::isChanged() {
  bool isChanged = false;
  std::unordered_map<unsigned short, vector<ForwardEntry> >::iterator it;
  for(it = forwardTable.begin(); it != forwardTable.end(); it ++){
    if(! (*it).second.empty() && (*it).second.at(0).TTL > MAX_DV_LS_TTL) {
      isChanged = true;
      if((*it).first != routerId){
        forwardTable[(*it).first].clear();
        forwardTable.erase((*it).first);
      }
    }
  }
  return isChanged;
}

unsigned short* ForwardTable::regenerateShortestPath() {
  unsigned short max_routerId = USHRT_MAX;
  std::unordered_map<unsigned short, vector<ForwardEntry> >::iterator it;
  for(it = forwardTable.begin(); it != forwardTable.end(); it ++) {
    if((*it).first > max_routerId){
      max_routerId = (*it).first;
    }
  }

  unsigned int dist[max_routerId + 1];
  unsigned short path_to[max_routerId + 1];
  bool inTree[max_routerId + 1];
  for(int i = 0; i < max_routerId + 1; i ++){
    dist[i] = UINT_MAX;
    inTree[i] = false;
    path_to[i] = USHRT_MAX;
  }
  dist[routerId] = 0;
  while (true) {
    unsigned int min_dist = UINT_MAX;
    unsigned short min_dist_id = USHRT_MAX;
    for(int i = 0; i < max_routerId + 1; i++){
      if(dist[i] < min_dist && inTree[i] == false) {
        min_dist = dist[i];
        min_dist_id = i;
      }
    }
    if(min_dist == UINT_MAX) break;
    inTree[min_dist_id] = true;
    if(forwardTable.find(min_dist_id) == forwardTable.end() || forwardTable[min_dist_id].empty()){
      continue;
    }

    for(unsigned int i = 0; i < forwardTable[min_dist_id].size(); i ++){
      if(forwardTable[min_dist_id].at(i).destID > max_routerId){
        printf("Invalid routerID \n");
        continue;
      }
      if(dist[forwardTable[min_dist_id].at(i).destID]>dist[min_dist_id]+forwardTable[min_dist_id].at(i).cost){
        //if find a shorter path
        dist[forwardTable[min_dist_id].at(i).destID]=dist[min_dist_id]+forwardTable[min_dist_id].at(i).cost;
        path_to[forwardTable[min_dist_id].at(i).destID]=min_dist_id;
      }
    }
  }

  return path_to;
}

bool ForwardTable::updateForwardTableAccordingToShortestPath(unsigned short* path_to){
  bool updated = false;
  std::unordered_map<unsigned short, vector<ForwardEntry> >::iterator it;
  for(it = forwardTable.begin(); it != forwardTable.end(); it ++) {
    unsigned short i=(*it).first;
    for(;path_to[i]!=routerId;i=path_to[i]){
      if(path_to[i]==USHRT_MAX){
        i=USHRT_MAX;
        break;
      }
    }

    if((*it).second.empty()){
      continue;
    }

    if((*it).second.at(0).nextHop!=i){
      (*it).second.at(0).nextHop=i;
      updated=true;
    }
  }
  return updated;
}

bool ForwardTable::updateForwardTableDV(unsigned short destID, unsigned short nextHop, unsigned short newCost,
                                        unsigned short oldCost) {
  bool changed = false;
  std::unordered_map<unsigned short, vector<ForwardEntry> >::iterator it;
  if(newCost == USHRT_MAX){
    for(it =forwardTable.begin(); it!=forwardTable.end(); it++){
      if((*it).second.empty())
        continue;
      if((*it).second.at(0).nextHop==destID){
        forwardTable[(*it).first].clear();
        forwardTable.erase((*it).first);
        changed = true;
      }
    }
    return changed;
  }
  if(forwardTable.find(destID) == forwardTable.end()){
    //entry not found, insert entry
    if(forwardTable[destID].empty())
      forwardTable[destID].push_back(ForwardEntry());
    forwardTable[destID].at(0).cost=newCost;
    forwardTable[destID].at(0).destID=destID;
    forwardTable[destID].at(0).TTL=0;
    forwardTable[destID].at(0).nextHop=nextHop;
    changed=true;
  }
  else if(forwardTable[destID].empty()){
    forwardTable[destID].push_back(ForwardEntry());
    forwardTable[destID].at(0).cost=newCost;
    forwardTable[destID].at(0).destID=destID;
    forwardTable[destID].at(0).TTL=0;
    forwardTable[destID].at(0).nextHop=nextHop;
    changed=true;
  }
  else if(forwardTable[destID].at(0).nextHop!=nextHop){
    if(newCost<forwardTable[destID].at(0).cost){
      //find a shorter path, change path
      forwardTable[destID].at(0).cost=newCost;
      forwardTable[destID].at(0).destID=destID;
      forwardTable[destID].at(0).TTL=0;
      forwardTable[destID].at(0).nextHop=nextHop;
      changed=true;
    }
    else{
      //current path is shorter, do nothing
      //return false;
    }
  }
  //refresh all entries whose via_hop == nextHop
  for(it=forwardTable.begin(); it!=forwardTable.end(); it++){
    if((*it).second.empty())
      continue;
    if((*it).second.at(0).nextHop==nextHop){ //&& {
      //if((*it).second.at(0).destID!=nextHop)
      (*it).second.at(0).cost=(*it).second.at(0).cost+newCost-oldCost;
      (*it).second.at(0).TTL=0;
      if(newCost!=oldCost)
        changed=true;
    }
  }

  return changed;
}

bool ForwardTable::updateForwardTableLS(unsigned short destID, unsigned short newCost) {
  for(unsigned int i=0; i<forwardTable[routerId].size(); i++){
    if(forwardTable[routerId].at(i).destID==destID){
      if(newCost==USHRT_MAX){
        forwardTable[routerId].erase(forwardTable[routerId].begin()+i);
        return true;
      }
      if(forwardTable[routerId].at(i).cost!=newCost){
        forwardTable[routerId].at(i).cost=newCost;
        return true;
      }
      else{
        return false;
      }
    }
  }
  if(newCost!=USHRT_MAX){
    forwardTable[routerId].push_back(ForwardEntry(destID,newCost,routerId,0));
    return true;
  }
  else{
    return false;
  }
}

int ForwardTable::size() {
  return forwardTable.size();
}

unordered_map<unsigned short, vector<ForwardEntry>>ForwardTable::getForwardTable() {
  return forwardTable;
}

unsigned int ForwardTable::getSeqNum() const {
  return seqNum;
}

void ForwardTable::increaseSeqNum() {
  this->seqNum ++;
}
