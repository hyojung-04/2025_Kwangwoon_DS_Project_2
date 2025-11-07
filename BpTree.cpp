//end

#include "BpTree.h"
#include "BpTreeIndexNode.h"
#include "BpTreeDataNode.h"

#include <vector>

bool BpTree::Insert(EmployeeData* newData) {
	
	// if tree is empty
	if (root == NULL) {
		// make new leaf node
		root = new BpTreeDataNode();
		// put new data inside map
		root->insertDataMap(newData->getName(), newData);
		return true;
	}

	// find where to insert (leaf)
	BpTreeNode* pLeaf = searchDataNode(newData->getName());
	BpTreeDataNode* pDataLeaf = dynamic_cast<BpTreeDataNode*>(pLeaf);
    if (pDataLeaf == NULL) return false; // not a data node

	// get data map in this leaf
	map<string, EmployeeData*>* pMap = pDataLeaf->getDataMap();

	// check if data already exists
	auto it = pMap->find(newData->getName());

	if (it != pMap->end()) {
		// if exists, update income
		it->second->setIncome(newData->getIncome());
		delete newData; // not needed anymore
	}
	else {
		// if not exists, insert new data
		pDataLeaf->insertDataMap(newData->getName(), newData);

        // if node is full
		if (excessDataNode(pDataLeaf)) {
			// split the node
			splitDataNode(pDataLeaf);
		}
	}

	return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
	// check if data node has too many data
	return pDataNode->getDataMap()->size() >= order;
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
	// check if index node has too many keys
	return pIndexNode->getIndexMap()->size() >= order;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) {
	BpTreeDataNode* pOldLeaf = dynamic_cast<BpTreeDataNode*>(pDataNode);
	if (pOldLeaf == NULL) return;

	// make new leaf
	BpTreeDataNode* pNewLeaf = new BpTreeDataNode();

	// get all keys
	map<string, EmployeeData*>* pMap = pOldLeaf->getDataMap();
	vector<string> keys;
	for (auto const& [key, val] : *pMap)
		keys.push_back(key);

	// find middle key
	int splitIndex = order / 2;
	string sMiddleKey = keys[splitIndex]; // key to push up

	// move half data to new leaf
	for (int i = splitIndex; i < keys.size(); i++) {
		string key = keys[i];
		EmployeeData* data = pMap->at(key);
		pNewLeaf->insertDataMap(key, data);
		pOldLeaf->deleteMap(key);
	}

	// connect linked list
	BpTreeNode* pOldNext = pOldLeaf->getNext();
	pNewLeaf->setNext(pOldNext);
	if (pOldNext != NULL) pOldNext->setPrev(pNewLeaf);
	pNewLeaf->setPrev(pOldLeaf);
	pOldLeaf->setNext(pNewLeaf);

	// connect with parent
	BpTreeNode* pParent = pOldLeaf->getParent();

	// if no parent (root split)
	if (pParent == NULL) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(pOldLeaf);
		newRoot->insertIndexMap(sMiddleKey, pNewLeaf);
		pOldLeaf->setParent(newRoot);
		pNewLeaf->setParent(newRoot);
		root = newRoot;
	}
	else {
		// normal split
		BpTreeIndexNode* pParentIndex = dynamic_cast<BpTreeIndexNode*>(pParent);
		if (pParentIndex == NULL) return;

		pParentIndex->insertIndexMap(sMiddleKey, pNewLeaf);
		pNewLeaf->setParent(pParentIndex);

		// if parent is full, split it too
		if (excessIndexNode(pParentIndex))
			splitIndexNode(pParentIndex);
	}
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
	BpTreeIndexNode* pOldIndex = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
	if (pOldIndex == NULL) return;

	// make new index node
	BpTreeIndexNode* pNewIndex = new BpTreeIndexNode();

	// get all keys
	map<string, BpTreeNode*>* pMap = pOldIndex->getIndexMap();
	vector<string> keys;
	for (auto const& [key, val] : *pMap)
		keys.push_back(key);

	// find middle key
	int splitIndex = order / 2;
	string sPromotedKey = keys[splitIndex]; // key to move up
	BpTreeNode* pPromotedChild = pMap->at(sPromotedKey);

	// remove middle key
	pOldIndex->deleteMap(sPromotedKey);

	// move right half keys to new node
	for (int i = splitIndex + 1; i < keys.size(); i++) {
		string key = keys[i];
		BpTreeNode* child = pMap->at(key);
		pNewIndex->insertIndexMap(key, child);
		pOldIndex->deleteMap(key);
	}

	// set most left child for new node
	pNewIndex->setMostLeftChild(pPromotedChild);
	pPromotedChild->setParent(pNewIndex);

	// update parent pointer for moved children
	for (auto const& [key, child] : *pNewIndex->getIndexMap())
		child->setParent(pNewIndex);

	// if root split
	BpTreeNode* pParent = pOldIndex->getParent();
	if (pParent == NULL) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(pOldIndex);
		newRoot->insertIndexMap(sPromotedKey, pNewIndex);
		pOldIndex->setParent(newRoot);
		pNewIndex->setParent(newRoot);
		root = newRoot;
	}
	else {
		// normal split
		BpTreeIndexNode* pParentIndex = dynamic_cast<BpTreeIndexNode*>(pParent);
		if (pParentIndex == NULL) return;

		pParentIndex->insertIndexMap(sPromotedKey, pNewIndex);
		pNewIndex->setParent(pParentIndex);

		if (excessIndexNode(pParentIndex))
			splitIndexNode(pParentIndex);
	}
}

BpTreeNode* BpTree::searchDataNode(string name) {
	BpTreeNode* pCur = root;

	// move down until data node
	while (dynamic_cast<BpTreeDataNode*>(pCur) == NULL) {
		BpTreeIndexNode* pIndex = dynamic_cast<BpTreeIndexNode*>(pCur);
        if (pIndex == NULL) break;

		map<string, BpTreeNode*>* pMap = pIndex->getIndexMap();
		BpTreeNode* pNextNode = pIndex->getMostLeftChild();

		for (auto const& [key, pChild] : *pMap) {
			if (name >= key) pNextNode = pChild;
			else break;
		}
		pCur = pNextNode;
	}
	return pCur;
}

void BpTree::searchRange(string start, string end) {
	// find start node
	BpTreeNode* pNode = searchDataNode(start);
	BpTreeDataNode* pCurLeaf = dynamic_cast<BpTreeDataNode*>(pNode);
	if (pCurLeaf == NULL) return;

	bool finished = false;

	// walk through linked leaf nodes
	while (pCurLeaf != NULL) {
		map<string, EmployeeData*>* pMap = pCurLeaf->getDataMap();

		for (auto const& [name, pData] : *pMap) {
			// stop if out of range
			if (name > end) { finished = true; break; }
			// print if in range
			if (name >= start) {
				*flog << pData->getName() << "/" 
                      << pData->getDeptNo() << "/" 
                      << pData->getID() << "/" 
                      << pData->getIncome() << "\n";
			}
		}
		if (finished) break;
		pCurLeaf = dynamic_cast<BpTreeDataNode*>(pCurLeaf->getNext());
	}
}

void BpTree::deleteTree(BpTreeNode* pNode) {
	if (pNode == NULL) return;

	// if data node
	BpTreeDataNode* pDataLeaf = dynamic_cast<BpTreeDataNode*>(pNode);
	if (pDataLeaf != NULL) {
		map<string, EmployeeData*>* pMap = pDataLeaf->getDataMap();
		for (auto const& [key, pData] : *pMap)
			delete pData;
	}
	else {
		// if index node
		BpTreeIndexNode* pIndexNode = dynamic_cast<BpTreeIndexNode*>(pNode);
		if (pIndexNode != NULL) {
			deleteTree(pIndexNode->getMostLeftChild());
			map<string, BpTreeNode*>* pMap = pIndexNode->getIndexMap();
			for (auto const& [key, pChild] : *pMap)
				deleteTree(pChild);
		}
	}
	delete pNode;
}
