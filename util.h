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
