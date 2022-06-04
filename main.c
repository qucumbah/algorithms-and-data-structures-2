#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "util.h"
#include "quicksort.h"
#include "mergesort.h"
#include "Queue.h"
#include "List.h"
#include "huffman.h"

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
  quicksort(array, sizeof(int), sizeof(array) / sizeof(int), compareInts);

  for (int i = 0; i < sizeof(array) / sizeof(int); i += 1) {
    printf("%d\n", array[i]);
  }
}

void testQueue() {
  int items[5] = {0, 1, 2, 3, 4};

  Queue* q = createQueueFromItems(items, sizeof(int), sizeof(items) / sizeof(int));
  printf("first: %d; last: %d; size: %d\n", *(int*)getFront(q), *(int*)getBack(q), q->nextIdx);
  rmFront(q);
  printf("first: %d; last: %d; size: %d\n", *(int*)getFront(q), *(int*)getBack(q), q->nextIdx);

  int someInt = 5;

  addBack(q, &someInt);
  printf("first: %d; last: %d; size: %d\n", *(int*)getFront(q), *(int*)getBack(q), q->nextIdx);
}

void testList() {
  List* list = createList(sizeof(int));
  for (int i = 0; i < 16; i += 1) {
    listAdd(list, &i);
  }

  assert(list->size == 16);
  assert(list->capacity == 16);

  int newValue = 16;
  listAdd(list, &newValue);

  assert(list->size == 17);
  assert(list->capacity == 32);

  for (int i = 0; i < 17; i += 1) {
    assert(listFind(list, &i, compareInts) == i);
  }

  int nonExistentValue = 180;
  assert(listFind(list, &nonExistentValue, compareInts) == -1);
}

void testHuffman() {
  huffmanEncode("arraySmol.bin", "f2.bin");
  huffmanDecode("f2.bin", "decoded.bin");
}

int main() {
  // testHuffman();
  testList();

  return 0;
}
