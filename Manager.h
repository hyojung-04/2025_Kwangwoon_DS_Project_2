#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>
#include <string>

class Manager {
private:
	char* cmd;
	BpTree* bptree;
	SelectionTree* stree;
	int bpOrder;
	bool isLoaded;

public:
	ifstream fin;
	ofstream flog;

	Manager(int bpOrder) {		//constructor
		this->bpOrder=bpOrder;
		bptree = new BpTree(&flog, bpOrder);
		stree = new SelectionTree(flog);
		isLoaded=false;
	}


	~Manager() {				//destructor
		delete bptree;
        delete stree;

        if (this->fin.is_open()) this->fin.close();
        if (this->flog.is_open()) this->flog.close();
	}

	void run(const char* command);
	void LOAD();
	void ADD_BP(std::string name,int dept_no,int id,int income);
	void SEARCH_BP_NAME(std::string name);
	void SEARCH_BP_RANGE(std::string start, std::string end);
	void PRINT_BP();
	void ADD_ST_DEPTNO(int dept_no);
	void ADD_ST_NAME(std::string name);
	void PRINT_ST(int n);
	void DELETE();

	void printErrorCode(int n);
	void printSuccessCode(std::string success);
};

