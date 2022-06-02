#include <stdio.h>
#include <stdbool.h>

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

void qsort(void* array, size_t itemSize, int arrayLength, Comparator* comparator) {
  qsort_inner(array, itemSize, 0, arrayLength - 1, comparator);
}

int compareInts(int* a, int* b) {
  return *a - *b;
}

int main() {
  int array[15] = {4, 22, 65, 67, 77, 87, 90, 23, 76, 90, 74, 5, 74, 89, 82};
  qsort(array, sizeof(int), sizeof(array) / sizeof(int), *compareInts);

  for (int i = 0; i < sizeof(array) / sizeof(int); i += 1) {
    printf("%d\n", array[i]);
  }

  return 0;
}
