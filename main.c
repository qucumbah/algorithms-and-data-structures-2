#include <stdio.h>
#include <stdbool.h>

#include "util.h"
#include "qsort.h"
#include "mergesort.h"

void testMergeSort() {
  int array[15] = {4, 22, 65, 67, 77, 87, 90, 23, 76, 90, 74, 5, 74, 89, 82};

  FILE* arrayStorage = fopen("array.bin", "wb");
  fwrite(array, sizeof(int), sizeof(array) / sizeof(int), arrayStorage);
  fclose(arrayStorage);
  system("hexdump ./array.bin");

  mergesort("array.bin", sizeof(int), sizeof(array) / sizeof(int), compareInts);
  system("hexdump ./array.bin");
}

void testQuicksort() {
  int array[15] = {4, 22, 65, 67, 77, 87, 90, 23, 76, 90, 74, 5, 74, 89, 82};
  quicksort(array, sizeof(int), sizeof(array) / sizeof(int), *compareInts);

  for (int i = 0; i < sizeof(array) / sizeof(int); i += 1) {
    printf("%d\n", array[i]);
  }
}

int main() {
  testMergeSort();

  return 0;
}
