#include "SelectionTree.h"
#include <iostream>
using namespace std;

// make empty tree with 8 department runs
void SelectionTree::setTree() {
    for (int i = 0; i < 8; i++) {
        run[i] = new SelectionTreeNode();
        run[i]->HeapInit(); // make empty heap
    }
    root = nullptr;
}

// insert new employee into proper heap
bool SelectionTree::Insert(EmployeeData* newData) {
    if (!newData) return false;

    int dept = newData->getDeptNo();
    int idx = (dept / 100) - 1;
    if (idx < 0 || idx >= 8) return false;

    // insert data into department heap
    run[idx]->getHeap()->Insert(newData);

    // update winner (max income among heaps)
    SelectionTreeNode* maxNode = nullptr;
    int maxIncome = -1;

    for (int i = 0; i < 8; i++) {
        EmployeeHeap* h = run[i]->getHeap();
        if (!h->IsEmpty()) {
            EmployeeData* top = h->Top();
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
    if (root == nullptr) return false;

    // delete top data in winner heap
    EmployeeHeap* winnerHeap = root->getHeap();
    if (winnerHeap->IsEmpty()) return false;

    winnerHeap->Delete();

    // find new winner
    SelectionTreeNode* newMax = nullptr;
    int maxIncome = -1;

    for (int i = 0; i < 8; i++) {
        EmployeeHeap* h = run[i]->getHeap();
        if (!h->IsEmpty()) {
            EmployeeData* top = h->Top();
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
    if (idx < 0 || idx >= 8) return false;

    EmployeeHeap* heap = run[idx]->getHeap();
    if (heap->IsEmpty()) return false;

    // temporary copy for printing (not delete)
    for (int i = 1; i <= heap->getdatanum(); i++) {
        EmployeeData* p = heap->getheapArr()[i];
        fout << p->getName() << "/" 
              << p->getDeptNo() << "/" 
              << p->getID() << "/" 
              << p->getIncome() << "\n";
    }

    return true;
}
