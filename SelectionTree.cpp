#include "SelectionTree.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// make empty tree with 8 department runs
void SelectionTree::setTree() {
  // 8 leaf (department heap)
  for (int i = 0; i < 8; i++) {
    run[i] = new SelectionTreeNode();
    run[i]->HeapInit();
  }

  // (Winner Tree)
  vector<SelectionTreeNode *> level;
  for (int i = 0; i < 8; i++)
    level.push_back(run[i]);

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

  root = level.front(); // set highest root
}

// insert new employee into proper heap
bool SelectionTree::Insert(EmployeeData *newData) {
  if (!newData)
    return false;

  int dept = newData->getDeptNo();
  int idx = (dept / 100) - 1;
  if (idx < 0 || idx >= 8)
    return false;

  // insert data into department heap
  run[idx]->getHeap()->Insert(newData);

  // update winner (max income among heaps)
  SelectionTreeNode *maxNode = nullptr;
  int maxIncome = -1;

  for (int i = 0; i < 8; i++) {
    EmployeeHeap *h = run[i]->getHeap();
    if (!h->IsEmpty()) {
      EmployeeData *top = h->Top();
      if (top->getIncome() > maxIncome) {
        maxIncome = top->getIncome();
        maxNode = run[i];
      }
    }
  }

  if (maxNode) {
    root = maxNode;
  }

  return true;
}

// delete top (max income) employee
bool SelectionTree::Delete() {
  if (root == nullptr)
    return false;

  // delete top data in winner heap
  EmployeeHeap *winnerHeap = root->getHeap();
  if (winnerHeap->IsEmpty())
    return false;

  winnerHeap->Delete();

  // find new winner
  SelectionTreeNode *newMax = nullptr;
  int maxIncome = -1;

  for (int i = 0; i < 8; i++) {
    EmployeeHeap *h = run[i]->getHeap();
    if (!h->IsEmpty()) {
      EmployeeData *top = h->Top();
      if (top->getIncome() > maxIncome) {
        maxIncome = top->getIncome();
        newMax = run[i];
      }
    }
  }

  root = newMax;
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

  // temporary copy for printing (not delete)
  for (int i = 1; i <= heap->getdatanum(); i++) {
    EmployeeData *p = heap->getheapArr()[i];
    fout << p->getName() << "/" << p->getDeptNo() << "/" << p->getID() << "/"
         << p->getIncome() << "\n";
  }

  return true;
}

// choose higher income for one node
void SelectionTree::updateWinner(SelectionTreeNode *node) {
  if (!node)
    return;

  SelectionTreeNode *left = node->getLeftChild();
  SelectionTreeNode *right = node->getRightChild();

  EmployeeData *ldata = nullptr;
  EmployeeData *rdata = nullptr;

  if (left && left->getHeap() && !left->getHeap()->IsEmpty())
    ldata = left->getHeap()->Top();
  if (right && right->getHeap() && !right->getHeap()->IsEmpty())
    rdata = right->getHeap()->Top();

  if (!ldata && !rdata) {
    node->setEmployeeData(nullptr);
    return;
  }
  if (!rdata || (ldata && ldata->getIncome() >= rdata->getIncome()))
    node->setEmployeeData(ldata);
  else
    node->setEmployeeData(rdata);
}

// rebuild whole selection tree
void SelectionTree::rebuildTree() {
  if (!root)
    return;

  queue<SelectionTreeNode *> q;
  q.push(root);
  while (!q.empty()) {
    SelectionTreeNode *node = q.front();
    q.pop();

    updateWinner(node);

    if (node->getLeftChild())
      q.push(node->getLeftChild());
    if (node->getRightChild())
      q.push(node->getRightChild());
  }
}
