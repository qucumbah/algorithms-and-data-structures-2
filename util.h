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

typedef void (MapFn)(void* from, void* to);

void* mapArray(void* from, size_t fromSize, size_t toSize, int arrayLength, MapFn mapFn) {
  void* to = malloc(arrayLength * toSize);

  for (int i = 0; i < arrayLength; i += 1) {
    mapFn(from + i * fromSize, to + i * toSize);
  }

  return to;
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

int binSearch(
  void* array,
  size_t itemSize,
  int arrayLength,
  void* itemToFind,
  Comparator comparator
) {
  assert(arrayLength > 0);
  int l = 0;
  int r = arrayLength - 1;

  while (l != r) {
    int m = (l + r) / 2;
    int comparisonResult = comparator(array + m * itemSize, itemToFind);
    if (comparisonResult < 0) {
      l = m + 1;
    } else if (comparisonResult > 0) {
      r = m;
    } else {
      return m;
    }
  }

  return (comparator(array + l * itemSize, itemToFind) >= 0) ? l : l + 1;
}

bool filesAreEqual(char* path1, char* path2) {
  FILE* file1 = fopen(path1, "rb");
  FILE* file2 = fopen(path2, "rb");

  char byte1;
  char byte2;

  fread(&byte1, 1, 1, file1);
  fread(&byte2, 1, 1, file2);

  while (!feof(file1) && !feof(file2)) {
    if (byte1 != byte2) {
      return false;
    }

    fread(&byte1, 1, 1, file1);
    fread(&byte2, 1, 1, file2);
  }

  return feof(file1) == feof(file2);
}
