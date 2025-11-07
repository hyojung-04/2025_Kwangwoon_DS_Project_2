#include "SelectionTree.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// build 8 department heaps and link tree
void SelectionTree::setTree() {
  for (int i = 0; i < 8; i++) {
    run[i] = new SelectionTreeNode();
    run[i]->HeapInit();
  }

  vector<SelectionTreeNode *> level;
  for (int i = 0; i < 8; i++)
    level.push_back(run[i]);

  // build winner tree by linking parents
  while (level.size() > 1) {
    vector<SelectionTreeNode *> next;
    for (int i = 0; i < level.size(); i += 2) {
      SelectionTreeNode *parent = new SelectionTreeNode();
      parent->setLeftChild(level[i]);
      if (i + 1 < level.size())
        parent->setRightChild(level[i + 1]);
      level[i]->setParent(parent);
      if (i + 1 < level.size())
        level[i + 1]->setParent(parent);
      next.push_back(parent);
    }
    level = next;
  }
  root = level.front();
  rebuildTree(); // initial winner
  updateWinner(root);
}

// insert new data to the proper heap
bool SelectionTree::Insert(EmployeeData *newData) {
  if (!newData)
    return false;

  int dept = newData->getDeptNo();
  int idx = (dept / 100) - 1;
  if (idx < 0 || idx >= 8)
    return false;

  // insert into heap
  run[idx]->getHeap()->Insert(newData);

  // update winners from leaf to root
  SelectionTreeNode *cur = run[idx];
  while (cur) {
    updateWinner(cur);
    cur = cur->getParent();
  }

  updateWinner(root);

  return true;
}

// delete top winner (root)
bool SelectionTree::Delete() {
  if (!root || !root->getEmployeeData())
    return false;

  EmployeeData *topData = root->getEmployeeData();
  if (!topData)
    return false;

  int dept = topData->getDeptNo();
  int idx = (dept / 100) - 1;
  if (idx < 0 || idx >= 8)
    return false;

  EmployeeHeap *h = run[idx]->getHeap();
  if (h->IsEmpty())
    return false;

  // remove top of heap
  h->Delete();

  // update winners up the tree
  SelectionTreeNode *cur = run[idx];
  while (cur) {
    updateWinner(cur);
    cur = cur->getParent();
  }

  updateWinner(root);

  return true;
}

// print employees of one department
bool SelectionTree::printEmployeeData(int dept_no) {
  int idx = (dept_no / 100) - 1;
  if (idx < 0 || idx >= 8)
    return false;

  EmployeeHeap *heap = run[idx]->getHeap();
  if (heap->IsEmpty())
    return false;

  fout << "========PRINT_ST========\n";

  vector<EmployeeData *> vec;
  for (int i = 1; i <= heap->getdatanum(); i++)
    vec.push_back(heap->getheapArr()[i]);

  // sort by income descending
  sort(vec.begin(), vec.end(), [](EmployeeData *a, EmployeeData *b) {
    return a->getIncome() > b->getIncome();
  });

  // print sorted
  for (auto p : vec) {
    fout << p->getName() << "/" << p->getDeptNo() << "/" << p->getID() << "/"
         << p->getIncome() << "\n";
  }

  fout << "====================\n\n";
  return true;
}

// update one node's winner (compare left & right)
void SelectionTree::updateWinner(SelectionTreeNode *node) {
  if (!node)
    return;

  SelectionTreeNode *left = node->getLeftChild();
  SelectionTreeNode *right = node->getRightChild();

  EmployeeData *ldata = nullptr;
  EmployeeData *rdata = nullptr;

  if (left) {
    if (left->getHeap() && !left->getHeap()->IsEmpty())
      ldata = left->getHeap()->Top();
    else
      ldata = left->getEmployeeData();
  }
  if (right) {
    if (right->getHeap() && !right->getHeap()->IsEmpty())
      rdata = right->getHeap()->Top();
    else
      rdata = right->getEmployeeData();
  }

  // choose higher income
  if (!ldata && !rdata)
    node->setEmployeeData(nullptr);
  else if (!rdata || (ldata && ldata->getIncome() >= rdata->getIncome()))
    node->setEmployeeData(ldata);
  else
    node->setEmployeeData(rdata);
}

// rebuild entire tree (bottom-up)
void SelectionTree::rebuildTree() {
  for (int i = 0; i < 8; i++) {
    SelectionTreeNode *cur = run[i];
    while (cur) {
      updateWinner(cur);
      cur = cur->getParent();
    }
  }
  updateWinner(root);
}
