#include "Manager.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void Manager::run(const char *command) {
  this->fin.open(command);

  // if command.txt not open
  if (!this->fin.is_open()) {
    // must change
    this->flog.open("log1.txt", ios::out);
    if (this->flog.is_open()) {
      printErrorCode(000);
    }
    return;
  }
  // must change
  this->flog.open("log1.txt", ios::out);
  if (!this->flog.is_open()) {
    fin.close();
    return;
  }

  string line;

  // read line by line
  while (getline(fin, line)) {
    stringstream ss(line);
    string cmd;

    // skip empty line
    if (!(ss >> cmd))
      continue;

    // LOAD
    if (cmd == "LOAD") {
      string extra;
      if (ss >> extra)
        printErrorCode(800);
      else
        LOAD();
    }

    // ADD_BP
    else if (cmd == "ADD_BP") {
      string name, s_dept, s_id, s_income, extra;
      if (!(ss >> name) || !(ss >> s_dept) || !(ss >> s_id) ||
          !(ss >> s_income))
        printErrorCode(200);
      else if (ss >> extra)
        printErrorCode(200);
      else {
        int dept_no = stoi(s_dept);
        int id = stoi(s_id);
        int income = stoi(s_income);
        ADD_BP(name, dept_no, id, income);
      }
    }

    // SEARCH_BP
    else if (cmd == "SEARCH_BP") {
      string arg1, arg2, extra;
      if (!(ss >> arg1))
        printErrorCode(300);
      else if (!(ss >> arg2))
        SEARCH_BP_NAME(arg1);
      else if (!(ss >> extra))
        SEARCH_BP_RANGE(arg1, arg2);
      else
        printErrorCode(300);
    }

    // ADD_ST
    else if (cmd == "ADD_ST") {
      string type, val, extra;
      if (!(ss >> type) || !(ss >> val))
        printErrorCode(500);
      else if (ss >> extra)
        printErrorCode(500);
      else {
        if (type == "dept_no") {
          try {
            ADD_ST_DEPTNO(stoi(val));
          } catch (...) {
            printErrorCode(500);
          }
        } else if (type == "name")
          ADD_ST_NAME(val);
        else
          printErrorCode(500);
      }
    }

    // PRINT_BP
    else if (cmd == "PRINT_BP") {
      string extra;
      if (ss >> extra)
        printErrorCode(400);
      else
        PRINT_BP();
    }

    // PRINT_ST
    else if (cmd == "PRINT_ST") {
      string s_dept, extra;
      if (!(ss >> s_dept))
        printErrorCode(600);
      else if (ss >> extra)
        printErrorCode(600);
      else {
        try {
          PRINT_ST(stoi(s_dept));
        } catch (...) {
          printErrorCode(600);
        }
      }
    }

    // DELETE
    else if (cmd == "DELETE") {
      string extra;
      if (ss >> extra)
        printErrorCode(700);
      else
        DELETE();
    }

    // EXIT
    else if (cmd == "EXIT") {
      printSuccessCode("EXIT");
      break;
    }

    // invalid command
    else
      printErrorCode(800);
  }
}

// LOAD employee.txt into B+ tree
void Manager::LOAD() {
  if (isLoaded) {
    printErrorCode(100);
    return;
  }

  ifstream employeeFile("employee.txt");
  if (!employeeFile.is_open()) {
    printErrorCode(100);
    return;
  }

  string line;
  while (getline(employeeFile, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string name, s_dept, s_id, s_income;
    getline(ss, name, '\t');
    getline(ss, s_dept, '\t');
    getline(ss, s_id, '\t');
    getline(ss, s_income);

    int dept_no = stoi(s_dept);
    int id = stoi(s_id);
    int income = stoi(s_income);

    EmployeeData *newData = new EmployeeData();
    newData->setData(name, dept_no, id, income);
    bptree->Insert(newData);
  }

  employeeFile.close();
  isLoaded = true;
  printSuccessCode("LOAD");
}

// ADD_BP: insert one employee into B+ tree
void Manager::ADD_BP(string name, int dept_no, int id, int income) {
  EmployeeData *newData = new EmployeeData();
  newData->setData(name, dept_no, id, income);
  bptree->Insert(newData);
  flog << "========ADD_BP========\n";
  flog << name << "/" << dept_no << "/" << id << "/" << income << "\n";
  flog << "====================\n\n";
}

// search by exact name
void Manager::SEARCH_BP_NAME(string name) {
  if (bptree->getRoot() == NULL) {
    printErrorCode(300);
    return;
  }

  BpTreeNode *pNode = bptree->searchDataNode(name);
  BpTreeDataNode *pLeaf = dynamic_cast<BpTreeDataNode *>(pNode);
  if (pLeaf == NULL) {
    printErrorCode(300);
    return;
  }

  map<string, EmployeeData *> *pMap = pLeaf->getDataMap();
  auto it = pMap->find(name);

  if (it == pMap->end())
    printErrorCode(300);
  else {
    flog << "========SEARCH_BP========\n";
    EmployeeData *pData = it->second;
    flog << pData->getName() << "/" << pData->getDeptNo() << "/"
         << pData->getID() << "/" << pData->getIncome() << "\n";
    flog << "====================\n\n";
  }
}

// search range [start, end]
void Manager::SEARCH_BP_RANGE(string start, string end) {
  if (bptree->getRoot() == NULL) {
    printErrorCode(300);
    return;
  }
  flog << "========SEARCH_BP========\n";
  bptree->searchRange(start, end);
  flog << "====================\n\n";
}

// add employees by dept_no into selection tree
void Manager::ADD_ST_DEPTNO(int dept_no) {
  if (bptree->getRoot() == NULL) {
    printErrorCode(500);
    return;
  }

  if (stree->getRoot() == NULL)
    stree->setTree();

  BpTreeNode *pNode = bptree->getRoot();
  while (pNode && dynamic_cast<BpTreeDataNode *>(pNode) == NULL)
    pNode = pNode->getMostLeftChild();

  if (pNode == NULL) {
    printErrorCode(500);
    return;
  }

  bool found = false;
  BpTreeDataNode *pLeaf = dynamic_cast<BpTreeDataNode *>(pNode);

  while (pLeaf) {
    map<string, EmployeeData *> *dataMap = pLeaf->getDataMap();
    for (auto const &pair : *dataMap) {
      string name = pair.first;
      EmployeeData *data = pair.second;
      if (data->getDeptNo() == dept_no) {
        stree->Insert(data);
        found = true;
      }
    }
    pLeaf = dynamic_cast<BpTreeDataNode *>(pLeaf->getNext());
  }

  if (found) {
    stree->rebuildTree();
    printSuccessCode("ADD_ST");
  } else {
    printErrorCode(500);
  }
}

// add one employee by name into selection tree
void Manager::ADD_ST_NAME(string name) {
  if (bptree->getRoot() == NULL) {
    printErrorCode(500);
    return;
  }

  if (stree->getRoot() == NULL)
    stree->setTree();

  BpTreeNode *pNode = bptree->searchDataNode(name);
  BpTreeDataNode *pLeaf = dynamic_cast<BpTreeDataNode *>(pNode);
  if (pLeaf == NULL) {
    printErrorCode(500);
    return;
  }

  map<string, EmployeeData *> *dataMap = pLeaf->getDataMap();
  auto it = dataMap->find(name);
  if (it == dataMap->end()) {
    printErrorCode(500);
    return;
  }

  stree->Insert(it->second);
  stree->rebuildTree();
  printSuccessCode("ADD_ST");
}

// delete top employee (max income)
void Manager::DELETE() {
  if (stree->getRoot() == NULL) {
    printErrorCode(700);
    return;
  }

  bool result = stree->Delete();
  if (!result) {
    printErrorCode(700);
    return;
  }

  stree->rebuildTree();
  printSuccessCode("DELETE");
}

// print all employees in B+ tree
void Manager::PRINT_BP() {
  if (bptree->getRoot() == NULL) {
    printErrorCode(400);
    return;
  }

  flog << "========PRINT_BP========\n";
  bptree->searchRange("", "{");
  flog << "====================\n\n";
}

// print one department data
void Manager::PRINT_ST(int n) {
  if (stree->getRoot() == NULL) {
    printErrorCode(600);
    return;
  }

  if (n < 100 || n > 800 || n % 100 != 0) {
    printErrorCode(600);
    return;
  }

  bool result = stree->printEmployeeData(n);

  if (!result) {
    printErrorCode(600);
    return;
  }
}

// print error message
void Manager::printErrorCode(int n) {
  flog << "========ERROR========\n";
  flog << n << "\n";
  flog << "=====================\n\n";
}

// print success message
void Manager::printSuccessCode(string success) {
  flog << "========" << success << "========\n";
  flog << "Success\n";
  flog << "====================\n\n";
}
