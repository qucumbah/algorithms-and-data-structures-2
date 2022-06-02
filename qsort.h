#include <stdlib.h>
#include <stdbool.h>

#include "util.h"

#pragma once

int partition(void* array, size_t itemSize, int l, int r, Comparator* comparator) {
  int pivotIdx = (l + r) / 2;

  while (true) {
    while ((*comparator)(&array[l * itemSize], &array[pivotIdx * itemSize]) < 0) {
      l += 1;
    }

    while ((*comparator)(&array[r * itemSize], &array[pivotIdx * itemSize]) > 0) {
      r -= 1;
    }

    if (l >= r) {
      return r;
    }

    swap(array, itemSize, l, r);

    if (pivotIdx == l) {
      pivotIdx = r;
    } else if (pivotIdx == r) {
      pivotIdx = l;
    }

    l += 1;
    r -= 1;
  }
}

void qsort_inner(void* array, size_t itemSize, int l, int r, Comparator* comparator) {
  if (r > l) {
    int m = partition(array, itemSize, l, r, comparator);
    qsort_inner(array, itemSize, l, m, comparator);
    qsort_inner(array, itemSize, m + 1, r, comparator);
  }
}

void quicksort(void* array, size_t itemSize, int arrayLength, Comparator* comparator) {
  qsort_inner(array, itemSize, 0, arrayLength - 1, comparator);
}
