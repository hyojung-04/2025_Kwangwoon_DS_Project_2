#include "BpTree.h"
#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include <vector>

// Insert new employee into B+ tree
bool BpTree::Insert(EmployeeData *newData) {
  // If tree is empty
  if (root == NULL) {
    root = new BpTreeDataNode();
    root->insertDataMap(newData->getName(), newData);
    return true;
  }

  // Find correct leaf node
  BpTreeNode *pLeaf = searchDataNode(newData->getName());
  BpTreeDataNode *pDataLeaf = dynamic_cast<BpTreeDataNode *>(pLeaf);
  if (pDataLeaf == NULL)
    return false;

  map<string, EmployeeData *> *pMap = pDataLeaf->getDataMap();

  // If name already exists → update income
  auto it = pMap->find(newData->getName());
  if (it != pMap->end()) {
    it->second->setIncome(newData->getIncome());
    delete newData;
  } else {
    // Insert new data
    pDataLeaf->insertDataMap(newData->getName(), newData);
    // Split if node is full
    if (excessDataNode(pDataLeaf))
      splitDataNode(pDataLeaf);
  }
  return true;
}

// Check if data node is full
bool BpTree::excessDataNode(BpTreeNode *pDataNode) {
  return pDataNode->getDataMap()->size() >= order;
}

// Check if index node is full
bool BpTree::excessIndexNode(BpTreeNode *pIndexNode) {
  return pIndexNode->getIndexMap()->size() >= order;
}

// Split a data node
void BpTree::splitDataNode(BpTreeNode *pDataNode) {
  BpTreeDataNode *pOldLeaf = dynamic_cast<BpTreeDataNode *>(pDataNode);
  if (pOldLeaf == NULL)
    return;

  // Create new leaf node
  BpTreeDataNode *pNewLeaf = new BpTreeDataNode();

  // Copy all keys from old leaf
  map<string, EmployeeData *> *pMap = pOldLeaf->getDataMap();
  vector<string> keys;
  for (map<string, EmployeeData *>::iterator it = pMap->begin();
       it != pMap->end(); ++it) {
    keys.push_back(it->first);
  }

  // Find split position
  int splitIndex = order / 2;
  string sMiddleKey = keys[splitIndex];

  // Move half data to new leaf
  for (int i = splitIndex; i < (int)keys.size(); i++) {
    string key = keys[i];
    EmployeeData *data = pMap->at(key);
    pNewLeaf->insertDataMap(key, data);
    pOldLeaf->deleteMap(key);
  }

  // Link two leaves
  BpTreeNode *pOldNext = pOldLeaf->getNext();
  pNewLeaf->setNext(pOldNext);
  if (pOldNext != NULL)
    pOldNext->setPrev(pNewLeaf);
  pNewLeaf->setPrev(pOldLeaf);
  pOldLeaf->setNext(pNewLeaf);

  // Handle parent connection
  BpTreeNode *pParent = pOldLeaf->getParent();

  // Case 1: No parent (split root)
  if (pParent == NULL) {
    BpTreeIndexNode *newRoot = new BpTreeIndexNode();
    newRoot->setMostLeftChild(pOldLeaf);
    newRoot->insertIndexMap(sMiddleKey, pNewLeaf);
    pOldLeaf->setParent(newRoot);
    pNewLeaf->setParent(newRoot);
    root = newRoot;
  } else {
    // Case 2: Has parent
    BpTreeIndexNode *pParentIndex = dynamic_cast<BpTreeIndexNode *>(pParent);
    if (pParentIndex == NULL)
      return;

    pParentIndex->insertIndexMap(sMiddleKey, pNewLeaf);
    pNewLeaf->setParent(pParentIndex);

    // Split parent if needed
    if (excessIndexNode(pParentIndex))
      splitIndexNode(pParentIndex);
  }
}

// Split an index node
void BpTree::splitIndexNode(BpTreeNode *pIndexNode) {
  BpTreeIndexNode *pOldIndex = dynamic_cast<BpTreeIndexNode *>(pIndexNode);
  if (pOldIndex == NULL)
    return;

  // Make new index node
  BpTreeIndexNode *pNewIndex = new BpTreeIndexNode();
  map<string, BpTreeNode *> *pMap = pOldIndex->getIndexMap();
  vector<string> keys;

  // Collect all keys
  for (map<string, BpTreeNode *>::iterator it = pMap->begin();
       it != pMap->end(); ++it) {
    keys.push_back(it->first);
  }

  // Find middle key
  int splitIndex = order / 2;
  string sPromotedKey = keys[splitIndex];
  BpTreeNode *pPromotedChild = pMap->at(sPromotedKey);

  // Remove middle key from old
  pOldIndex->deleteMap(sPromotedKey);

  // Move right half to new index node
  for (int i = splitIndex + 1; i < (int)keys.size(); i++) {
    string key = keys[i];
    BpTreeNode *child = pMap->at(key);
    pNewIndex->insertIndexMap(key, child);
    pOldIndex->deleteMap(key);
  }

  // Connect children
  pNewIndex->setMostLeftChild(pPromotedChild);
  pPromotedChild->setParent(pNewIndex);

  map<string, BpTreeNode *> *newMap = pNewIndex->getIndexMap();
  for (map<string, BpTreeNode *>::iterator it = newMap->begin();
       it != newMap->end(); ++it) {
    it->second->setParent(pNewIndex);
  }

  // Case 1: old node was root
  BpTreeNode *pParent = pOldIndex->getParent();
  if (pParent == NULL) {
    BpTreeIndexNode *newRoot = new BpTreeIndexNode();
    newRoot->setMostLeftChild(pOldIndex);
    newRoot->insertIndexMap(sPromotedKey, pNewIndex);
    pOldIndex->setParent(newRoot);
    pNewIndex->setParent(newRoot);
    root = newRoot;
  } else {
    // Case 2: normal internal split
    BpTreeIndexNode *pParentIndex = dynamic_cast<BpTreeIndexNode *>(pParent);
    if (pParentIndex == NULL)
      return;

    pParentIndex->insertIndexMap(sPromotedKey, pNewIndex);
    pNewIndex->setParent(pParentIndex);

    if (excessIndexNode(pParentIndex))
      splitIndexNode(pParentIndex);
  }
}

// Find leaf node that contains (or should contain) name
BpTreeNode *BpTree::searchDataNode(string name) {
  BpTreeNode *pCur = root;

  while (dynamic_cast<BpTreeDataNode *>(pCur) == NULL) {
    BpTreeIndexNode *pIndex = dynamic_cast<BpTreeIndexNode *>(pCur);
    if (pIndex == NULL)
      break;

    map<string, BpTreeNode *> *pMap = pIndex->getIndexMap();
    BpTreeNode *pNextNode = pIndex->getMostLeftChild();

    for (map<string, BpTreeNode *>::iterator it = pMap->begin();
         it != pMap->end(); ++it) {
      if (name >= it->first)
        pNextNode = it->second;
      else
        break;
    }
    pCur = pNextNode;
  }
  return pCur;
}

// Print all data in range [start, end]
void BpTree::searchRange(string start, string end) {
  BpTreeNode *pNode = searchDataNode(start);
  BpTreeDataNode *pCurLeaf = dynamic_cast<BpTreeDataNode *>(pNode);
  if (pCurLeaf == NULL)
    return;

  bool finished = false;

  // Go through leaf nodes
  while (pCurLeaf != NULL) {
    map<string, EmployeeData *> *pMap = pCurLeaf->getDataMap();

    for (map<string, EmployeeData *>::iterator it = pMap->begin();
         it != pMap->end(); ++it) {
      string name = it->first;
      EmployeeData *pData = it->second;

      // Stop if out of range
      if (name.compare(0, end.size(), end) > 0) {
        finished = true;
        break;
      }

      // Print if in range
      if (name >= start) {
        *flog << pData->getName() << "/" << pData->getDeptNo() << "/"
              << pData->getID() << "/" << pData->getIncome() << "\n";
      }
    }
    if (finished)
      break;

    // Go to next leaf
    pCurLeaf = dynamic_cast<BpTreeDataNode *>(pCurLeaf->getNext());
  }
}

// Delete whole tree recursively
void BpTree::deleteTree(BpTreeNode *pNode) {
  if (pNode == NULL)
    return;

  // If it's a data leaf
  BpTreeDataNode *pDataLeaf = dynamic_cast<BpTreeDataNode *>(pNode);
  if (pDataLeaf != NULL) {
    map<string, EmployeeData *> *pMap = pDataLeaf->getDataMap();
    for (map<string, EmployeeData *>::iterator it = pMap->begin();
         it != pMap->end(); ++it)
      delete it->second;
  } else {
    // If it's an index node
    BpTreeIndexNode *pIndexNode = dynamic_cast<BpTreeIndexNode *>(pNode);
    if (pIndexNode != NULL) {
      deleteTree(pIndexNode->getMostLeftChild());
      map<string, BpTreeNode *> *pMap = pIndexNode->getIndexMap();
      for (map<string, BpTreeNode *>::iterator it = pMap->begin();
           it != pMap->end(); ++it)
        deleteTree(it->second);
    }
  }
  delete pNode;
}
