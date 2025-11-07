#pragma once
#include "EmployeeHeap.h"

// Insert new data into heap
void EmployeeHeap::Insert(EmployeeData *data) {
  // if heap is full, resize array
  if (datanum + 1 >= maxCapacity) {
    ResizeArray();
  }

  // insert at next position
  heapArr[++datanum] = data;

  // move up to keep max-heap property
  UpHeap(datanum);
}

// return top element (max income)
EmployeeData *EmployeeHeap::Top() {
  if (IsEmpty())
    return NULL;
  return heapArr[1];
}

// delete top (root) and rearrange heap
void EmployeeHeap::Delete() {
  if (IsEmpty())
    return;

  // move last element to root
  heapArr[1] = heapArr[datanum];
  datanum--;

  // fix heap from root down
  DownHeap(1);
}

// check if heap is empty
bool EmployeeHeap::IsEmpty() { return datanum == 0; }

// move up the element to correct position
void EmployeeHeap::UpHeap(int index) {
  while (index > 1) {
    int parent = index / 2;

    // if child income > parent income, swap
    if (heapArr[index]->getIncome() > heapArr[parent]->getIncome()) {
      EmployeeData *temp = heapArr[index];
      heapArr[index] = heapArr[parent];
      heapArr[parent] = temp;
      index = parent;
    } else
      break;
  }
}

// move down the element to correct position
void EmployeeHeap::DownHeap(int index) {
  while (index * 2 <= datanum) {
    int left = index * 2;
    int right = index * 2 + 1;
    int larger = left;

    // choose larger child
    if (right <= datanum &&
        heapArr[right]->getIncome() > heapArr[left]->getIncome()) {
      larger = right;
    }

    // if parent smaller than child, swap
    if (heapArr[index]->getIncome() < heapArr[larger]->getIncome()) {
      EmployeeData *temp = heapArr[index];
      heapArr[index] = heapArr[larger];
      heapArr[larger] = temp;
      index = larger;
    } else
      break;
  }
}

// make heap array bigger
void EmployeeHeap::ResizeArray() {
  int newCapacity = maxCapacity * 2;
  EmployeeData **newArr = new EmployeeData *[newCapacity];

  // copy old data
  for (int i = 0; i <= datanum; i++) {
    newArr[i] = heapArr[i];
  }

  // free old memory
  delete[] heapArr;

  heapArr = newArr;
  maxCapacity = newCapacity;
}

// constructor
EmployeeHeap::EmployeeHeap() {
  datanum = 0;
  heapArr = new EmployeeData *[maxCapacity];
  for (int i = 0; i < maxCapacity; i++)
    heapArr[i] = NULL;
}

// destructor
EmployeeHeap::~EmployeeHeap() {
  // only delete array, not EmployeeData itself (B+Tree frees it)
  delete[] heapArr;
}
