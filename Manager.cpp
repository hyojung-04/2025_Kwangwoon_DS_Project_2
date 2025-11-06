#include "Manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

void Manager::run(const char* command) {
	this->fin.open(command);

	// command.txt did not open.
	if(!this->fin.is_open()){
		this->flog.open("log.txt", ios::out);
		if(this->flog.is_open()){
			printErrorCode(000);
		}
		return;
	}

	this->flog.open("log.txt", ios::out);
	if(!this->flog.is_open()){
		fin.close();
		return;
	}

	string line;

	while (getline(fin, line)) {
        stringstream ss(line);
        string cmd;

		// skip empty line
        if (!(ss >> cmd)) {
            continue;
        }

		// LOAD
        if (cmd == "LOAD") {
            string extra;
            if (ss >> extra) {
                printErrorCode(800);
            } else {
                LOAD();
            }
        }
		// ADD_BP
        else if (cmd == "ADD_BP") {
            string name, s_dept, s_id, s_income, extra;
			// not enough arguments provided
            if (!(ss >> name) || !(ss >> s_dept) || !(ss >> s_id) || !(ss >> s_income)) {
                printErrorCode(200); 
            } else if (ss >> extra) {
                printErrorCode(200);
            } else {
                    int dept_no = stoi(s_dept);
                    int id = stoi(s_id);
                    int income = stoi(s_income);
                    
                    ADD_BP(name, dept_no, id, income);
               
            }
        }
        // SEARCH_BP
        else if (cmd == "SEARCH_BP") {
            string arg1, arg2, extra;
            // no argument
            if (!(ss >> arg1)) {
                printErrorCode(300);
            } 
            // one argument, search name
            else if (!(ss >> arg2)) {
                SEARCH_BP_NAME(arg1);
            } else if (!(ss >> extra)) {
                // two arguments, search range
                SEARCH_BP_RANGE(arg1, arg2);
            } else {
                printErrorCode(300); // over three arguments
            }
        }
        else if (cmd == "ADD_ST") {
            string type, val, extra;
            // need two arguments
            if (!(ss >> type) || !(ss >> val)) {
                printErrorCode(500); // need more argument
            } else if (ss >> extra) {
                printErrorCode(500); // too much argument
            } else {
                if (type == "dept_no") { 
                    try {
                        ADD_ST_DEPTNO(stoi(val));
                    } catch (...) {
                        printErrorCode(500); // wrong number
                    }
                } else if (type == "name") {
                    ADD_ST_NAME(val);
                } else {
                    printErrorCode(500); // wrong type
                }
            }
        }
        else if (cmd == "PRINT_BP") {
            string extra;
            if (ss >> extra) { // no argument
                printErrorCode(400); 
            } else {
                PRINT_BP();
            }
        }
        else if (cmd == "PRINT_ST") {
            string s_dept, extra;
            // need one more argument
            if (!(ss >> s_dept)) {
                printErrorCode(600); // need more argument
            } else if (ss >> extra) {
                printErrorCode(600); // over argument
            } else {
                try {
                    // excute PRINT_ST
                    PRINT_ST(stoi(s_dept));
                } catch (...) {
                    printErrorCode(600); // wrong number
                }
            }
        }
        else if (cmd == "DELETE") {
            string extra;
            if (ss >> extra) { // no argument
                printErrorCode(700); 
            } else {
                DELETE();
            }
        }
        else if (cmd == "EXIT") {
            printSuccessCode("EXIT");
            break; // end of while loop
        }
        else {
            printErrorCode(800); // wrong instruction
        }
    }
	


}

void Manager::LOAD() {
    // Data already exist in B+ Tree
    if (isLoaded) {
        printErrorCode(100);
        return;
    }

    ifstream employeeFile("employee.txt");

	// no file
	if(!employeeFile.is_open()){
		printErrorCode(100);
		return;
	}

    string line;
   
    while (getline(employeeFile, line)) {
        if (line.empty()) continue; // skip empty line

        stringstream ss(line);
        string name, s_dept, s_id, s_income;
        int dept_no, id, income;

        // seperate line
        getline(ss, name, '\t');
        getline(ss, s_dept, '\t');
        getline(ss, s_id, '\t');
        getline(ss, s_income); 

        // change string to int
        dept_no = stoi(s_dept);
        id = stoi(s_id);
        income = stoi(s_income);

        // construct EmployeeData object
        EmployeeData* newData = new EmployeeData();
        newData->setData(name, dept_no, id, income);

        // 5. B+ Tree에 삽입 (이름 기준 정렬) [cite: 32]
        // (BpTree.cpp의 insert 함수가 구현되어 있어야 함)
        bptree->Insert(newData);
    }

    employeeFile.close();
    isLoaded = true; // 로드 완료 플래그 설정
    
    printSuccessCode("LOAD");

}

void Manager::ADD_BP(string name,int dept_no,int id,int income) {

}

void Manager::SEARCH_BP_NAME(string name) {

}

void Manager::SEARCH_BP_RANGE(string start, string end) {

}

void Manager::ADD_ST_DEPTNO(int dept_no) {

}

void Manager::ADD_ST_NAME(string name) {

}

void Manager::PRINT_BP() {

}

void Manager::PRINT_ST(int n) {

}

void Manager::DELETE() {

}

void Manager::printErrorCode(int n) {
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success) {
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}