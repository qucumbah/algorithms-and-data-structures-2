#include <stdlib.h>
#include <string.h>

#include "util.h"

#pragma once

void radixsort(void* array, size_t itemSize, int arrayLength, KeyExtractor* keyExtractor) {
  void* buffer = malloc(itemSize * arrayLength);
  for (int bit = 0; bit < 32; bit += 1) {
    int zerosIndex = 0;
    int onesIndex = 0;

    int checkBit = 1 << bit;

    for (int i = 0; i < arrayLength; i += 1) {
      if ((keyExtractor(array + i * itemSize) & checkBit) == 0) {
        onesIndex += 1;
      }
    }

    for (int i = 0; i < arrayLength; i += 1) {
      if ((keyExtractor(array + i * itemSize) & checkBit) == 0) {
        memmove(buffer + zerosIndex * itemSize, array + i * itemSize, itemSize);
        zerosIndex += 1;
      } else {
        memmove(buffer + onesIndex * itemSize, array + i * itemSize, itemSize);
        onesIndex += 1;
      }
    }

    memmove(array, buffer, itemSize * arrayLength);
  }
}
