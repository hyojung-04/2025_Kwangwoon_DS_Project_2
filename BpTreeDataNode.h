#pragma once
#ifndef _BpTreeDataNode_H_
#define _BpTreeDataNode_H_

#include "BpTreeNode.h"

class BpTreeDataNode : public BpTreeNode {
private:
  map<string, EmployeeData *> mapData;
  BpTreeNode *pNext;
  BpTreeNode *pPrev;

public:
  BpTreeDataNode() {
    pNext = NULL;
    pPrev = NULL;
  }
  ~BpTreeDataNode() {
    for (auto &pair : mapData)
      delete pair.second;
    mapData.clear();
  }
  void setNext(BpTreeNode *pN) { pNext = pN; }
  void setPrev(BpTreeNode *pN) { pPrev = pN; }
  BpTreeNode *getNext() { return pNext; }
  BpTreeNode *getPrev() { return pPrev; }

  void insertDataMap(string name, EmployeeData *pN) {
    auto it = mapData.find(name);
    if (it != mapData.end()) {
      delete it->second;
      it->second = pN;
    } else {
      mapData[name] = pN;
    }
  }
  void deleteMap(string name) {
    auto it = mapData.find(name);
    if (it != mapData.end()) {
      delete it->second;
      mapData.erase(it);
    }
  }
  map<string, EmployeeData *> *getDataMap() { return &mapData; }
};

#endif
