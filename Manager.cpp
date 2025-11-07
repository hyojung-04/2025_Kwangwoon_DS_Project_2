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

void Manager::ADD_BP(string name, int dept_no, int id, int income) {
    // ... (EmployeeData 생성, bptree->Insert) ...

    // 3. 성공 코드 및 결과 출력
    printSuccessCode("ADD_BP");
    flog << name << "/" << dept_no << "/" << id << "/" << income << "\n";
    flog << "====================\n\n";
}


void Manager::SEARCH_BP_NAME(string name) {
    // 1. B+ Tree가 비어있는지 확인
    if (bptree->getRoot() == NULL) {
        printErrorCode(300); // [cite: 103]
        return;
    }

    // 2. 이름이 속한 리프 노드(DataNode) 탐색
    BpTreeNode* pNode = bptree->searchDataNode(name);
    BpTreeDataNode* pLeaf = dynamic_cast<BpTreeDataNode*>(pNode);

    if (pLeaf == NULL) { // 혹시 모를 오류 방지
        printErrorCode(300);
        return;
    }

    // 3. 리프 노드의 map에서 정확한 이름 탐색
    map<string, EmployeeData*>* pMap = pLeaf->getDataMap();
    auto it = pMap->find(name);

    // 4. 결과 처리
    if (it == pMap->end()) {
        // 맵에 이름이 없음
        printErrorCode(300); // [cite: 103]
    } else {
        // 찾았음. 데이터 출력 [cite: 103]
        EmployeeData* pData = it->second;
        printSuccessCode("SEARCH_BP");
        flog << pData->getName() << "/" 
              << pData->getDeptNo() << "/" 
              << pData->getID() << "/" 
              << pData->getIncome() << "\n";
        flog << "====================\n\n";
    }
}


void Manager::SEARCH_BP_RANGE(string start, string end) {
    // 1. B+ Tree가 비어있는지 확인
    if (bptree->getRoot() == NULL) {
        printErrorCode(300); // [cite: 103]
        return;
    }

    // 2. 성공 코드 출력
    printSuccessCode("SEARCH_BP");

    // 3. B+ Tree의 범위 탐색 함수 호출 (이 함수가 flog에 직접 씀)
    bptree->searchRange(start, end);

    // 4. 꼬리말 추가
    flog << "====================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no) {
    if (bptree->getRoot() == NULL) {
        printErrorCode(500);
        return;
    }

    if (stree->getRoot() == NULL) // only once
        stree->setTree();

    BpTreeNode* pNode = bptree->getRoot();
    while (pNode && dynamic_cast<BpTreeDataNode*>(pNode) == NULL)
        pNode = pNode->getMostLeftChild();

    if (pNode == NULL) {
        printErrorCode(500);
        return;
    }

    bool found = false;
    BpTreeDataNode* pLeaf = dynamic_cast<BpTreeDataNode*>(pNode);

    while (pLeaf) {
        map<string, EmployeeData*>* dataMap = pLeaf->getDataMap();
        for (auto const& [name, data] : *dataMap) {
            if (data->getDeptNo() == dept_no) {
                stree->Insert(data);
                found = true;
            }
        }
        pLeaf = dynamic_cast<BpTreeDataNode*>(pLeaf->getNext());
    }

    if (found) {
        stree->rebuildTree(); // <== added
        printSuccessCode("ADD_ST");
    } else {
        printErrorCode(500);
    }
}

void Manager::ADD_ST_NAME(string name) {
    if (bptree->getRoot() == NULL) {
        printErrorCode(500);
        return;
    }

    if (stree->getRoot() == NULL)
        stree->setTree();

    BpTreeNode* pNode = bptree->searchDataNode(name);
    BpTreeDataNode* pLeaf = dynamic_cast<BpTreeDataNode*>(pNode);

    if (pLeaf == NULL) {
        printErrorCode(500);
        return;
    }

    map<string, EmployeeData*>* dataMap = pLeaf->getDataMap();
    auto it = dataMap->find(name);
    if (it == dataMap->end()) {
        printErrorCode(500);
        return;
    }

    stree->Insert(it->second);
    stree->rebuildTree(); // <== added
    printSuccessCode("ADD_ST");
}

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

    stree->rebuildTree(); // <== added
    printSuccessCode("DELETE");
}



void Manager::ADD_ST_NAME(string name) {
    // 1. B+ Tree 비어있으면 에러
    if (bptree->getRoot() == NULL) {
        printErrorCode(500);
        return;
    }

    // 2. Selection Tree 초기화
    stree->setTree();

    // 3. 이름이 있는 리프 노드 탐색
    BpTreeNode* pNode = bptree->searchDataNode(name);
    BpTreeDataNode* pLeaf = dynamic_cast<BpTreeDataNode*>(pNode);

    if (pLeaf == NULL) {
        printErrorCode(500);
        return;
    }

    // 4. 맵에서 이름 검색
    map<string, EmployeeData*>* dataMap = pLeaf->getDataMap();
    auto it = dataMap->find(name);

    if (it == dataMap->end()) {
        printErrorCode(500);
        return;
    }

    // 5. Selection Tree에 삽입
    stree->Insert(it->second);

    // 6. 성공 출력
    printSuccessCode("ADD_ST");
}


void Manager::PRINT_BP() {
    // 1. B+ Tree가 비어있는지 확인
    if (bptree->getRoot() == NULL) {
        printErrorCode(400); // [cite: 105]
        return;
    }

    // 2. 성공 코드 출력
    printSuccessCode("PRINT_BP");

    // 3. 전체 범위로 searchRange 호출
    //    (시작: 빈 문자열, 끝: ASCII에서 'z'보다 큰 '{' 사용)
    bptree->searchRange("", "{"); 

    // 4. 꼬리말 추가
    flog << "====================\n\n";
}

void Manager::PRINT_ST(int n) {
    // 1. SelectionTree가 비어있으면 에러
    if (stree->getRoot() == NULL) {
        printErrorCode(600);
        return;
    }

    // 2. 부서 코드가 유효한지 확인
    if (n < 100 || n > 800 || n % 100 != 0) {
        printErrorCode(600);
        return;
    }

    // 3. 부서에 해당하는 데이터 출력
    bool result = stree->printEmployeeData(n);

    if (!result) {
        printErrorCode(600);
        return;
    }

    // 4. 출력 성공 시
    printSuccessCode("PRINT_ST");
}


void Manager::DELETE() {
    // 1. SelectionTree 비어있으면 에러
    if (stree->getRoot() == NULL) {
        printErrorCode(700);
        return;
    }

    // 2. SelectionTree에서 삭제 시도
    bool result = stree->Delete();

    // 3. 실패 시
    if (!result) {
        printErrorCode(700);
        return;
    }

    // 4. 성공 시
    printSuccessCode("DELETE");
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