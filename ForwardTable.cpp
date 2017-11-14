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
