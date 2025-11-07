#pragma once
#include "SelectionTreeNode.h"
#include <fstream>

class SelectionTree {
private:
    SelectionTreeNode* root;
    std::ofstream& fout;
    SelectionTreeNode* run[8];

public:
    SelectionTree(std::ofstream& fout) : fout(fout) {
    }

    ~SelectionTree() {

    }

    void setRoot(SelectionTreeNode* pN) { this->root = pN; }
    SelectionTreeNode* getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);
};