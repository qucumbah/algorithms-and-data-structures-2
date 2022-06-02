#include <stdlib.h>

#pragma once

typedef int (Comparator)(const void* a, const void* b);

void swap(void* array, size_t itemSize, int a, int b) {
  char tempByte;
  char* arrayBytes = ((char*)array);

  for (size_t i = 0; i < itemSize; i += 1) {
    tempByte = arrayBytes[a * itemSize + i];
    arrayBytes[a * itemSize + i] = arrayBytes[b * itemSize + i];
    arrayBytes[b * itemSize + i] = tempByte;
  }
}

int compareInts(const void* a, const void* b) {
  return *(int*)a - *(int*)b;
}
