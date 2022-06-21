#include <stdlib.h>
#include <stdbool.h>

#include "util.h"

#pragma once

int leftChild(int index) {
  return index * 2 + 1;
}

int rightChild(int index) {
  return index * 2 + 2;
}

void heapFix(void* array, size_t itemSize, int arrayLength, int where, Comparator* comparator) {
  int leftChildIndex = leftChild(where);
  int rightChildIndex = rightChild(where);

  if (leftChildIndex >= arrayLength) {
    return;
  }

  if (rightChildIndex == arrayLength) {
    // Only one child (left), compare it with parent and swap if needed
    if (comparator(array + where * itemSize, array + leftChildIndex * itemSize) < 0) {
      swap(array, itemSize, where, leftChildIndex);
    }

    return;
  }

  int maxChildIndex = (comparator(array + leftChildIndex * itemSize, array + rightChildIndex * itemSize) > 0) ? leftChildIndex : rightChildIndex;

  if (comparator(array + where * itemSize, array + maxChildIndex * itemSize) < 0) {
    swap(array, itemSize, where, maxChildIndex);
    heapFix(array, itemSize, arrayLength, maxChildIndex, comparator);
  }
}

void heapsort(void* array, size_t itemSize, int arrayLength, Comparator* comparator) {
  for (int i = arrayLength - 1; i >= 0; i -= 1) {
    heapFix(array, itemSize, arrayLength, i, comparator);
  }

  for (int i = arrayLength - 1; i >= 0; i -= 1) {
    swap(array, itemSize, 0, i);
    heapFix(array, itemSize, i, 0, comparator);
  }
}
