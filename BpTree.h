// end

#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"
#include <fstream>

class BpTree {
private:
  BpTreeNode *root;
  int order; // m children
  ofstream *flog;

public:
  BpTree(ofstream *fout, int order = 3) {
    this->root = NULL;
    this->order = order;
    this->flog = fout;
  }

  ~BpTree() { deleteTree(root); }

  /* essential */
  bool Insert(EmployeeData *newData);
  bool excessDataNode(BpTreeNode *pDataNode);
  bool excessIndexNode(BpTreeNode *pIndexNode);
  void splitDataNode(BpTreeNode *pDataNode);
  void splitIndexNode(BpTreeNode *pIndexNode);
  BpTreeNode *getRoot() { return root; }
  BpTreeNode *searchDataNode(string name);
  void searchRange(string start, string end);
  void deleteTree(BpTreeNode *pNode);
};

#endif
