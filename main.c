#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "util.h"
#include "quicksort.h"
#include "mergesort.h"
#include "Queue.h"
#include "List.h"
#include "HashTable.h"
#include "blockFile.h"
#include "indexFile.h"
#include "sparceIndex.h"
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

void testHashTable() {
  List* keys = createList(sizeof(char*));
  List* values = createList(sizeof(int));

  FILE* data = fopen("./hashTableData.txt", "r");
  while (!feof(data)) {
    char* keyBuffer = malloc(256);
    int valueBuffer;

    fscanf(data, "%s %d", keyBuffer, &valueBuffer);

    listAdd(keys, &keyBuffer);
    listAdd(values, &valueBuffer);
  }

  HashTable* table = createHashTable(sizeof(int));

  for (int i = 0; i < keys->size; i += 1) {
    char* key = *(char**)listGet(keys, i);
    int value = *(int*)listGet(values, i);
    printf("Adding (%s, %d) to the hash table\n", key, value);
    hashTableAdd(table, key, &value);
  }

  for (int i = 0; i < keys->size; i += 1) {
    char* key = *(char**)listGet(keys, i);
    int value = *(int*)hashTableGet(table, key);
    printf("Item with key %s: %d\n", key, value);
  }
}

void testBlockFile() {
  char blockFilePath[] = "data/block2.bin";
  createBlockFile(blockFilePath);

  List* keys = createList(sizeof(int));
  List* values = createList(sizeof(char*));

  FILE* data = fopen("./hashTableData.txt", "r");
  while (!feof(data)) {
    int keyBuffer;
    char* valueBuffer = malloc(256);

    fscanf(data, "%s %d", valueBuffer, &keyBuffer);

    listAdd(keys, &keyBuffer);
    listAdd(values, &valueBuffer);
  }

  for (int i = 0; i < keys->size; i += 1) {
    int key = *(int*)listGet(keys, i);
    char* value = *(char**)listGet(values, i);
    printf("Adding (%d, %s) to the block\n", key, value);
    writeToBlock(blockFilePath, key, value, strlen(value));
  }

  for (int i = 0; i < keys->size; i += 1) {
    int key = *(int*)listGet(keys, i);
    char* value = *(char**)listGet(values, i);

    BlockFileEntry readValue = readFromBlock(blockFilePath, key);

    assert(readValue.size == strlen(value));

    for (int i = 0; i < readValue.size; i += 1) {
      assert(((char*)readValue.data)[i] == value[i]);
    }

    printf("Successfully read %s from block file with key %d\n", value, key);
  }
}

void testIndexSearch() {
  char* indexFileLocation = "data/index.bin";

  createIndexFile(indexFileLocation);

  expandIndexFile(indexFileLocation, 15);
  expandIndexFile(indexFileLocation, 20);
  expandIndexFile(indexFileLocation, 3);

  Block block = getBlockForKey(indexFileLocation, 35);
  printf("%d (%d-%d)\n", block.number, block.sizeDescriptor.start, block.sizeDescriptor.end);
}

void testSparceIndex() {
  sparceIndexInit();

  sparceIndexExpand(5);
  sparceIndexExpand(5);
  sparceIndexExpand(5);
  
  for (int i = 0; i < 15; i += 1) {
    sparceIndexWrite(i, &i, sizeof(int));
  }

  for (int i = 0; i < 15; i += 1) {
    BlockFileEntry result = sparceIndexRead(i);
    assert(*(int*)result.data == i);
  }
}

void testHuffmanEntry(void* input) {
  char compressed[256];
  char decompressed[256];

  sprintf(compressed, "./temp/compressed_%s.bin", input);
  sprintf(decompressed, "./temp/decompressed_%s", input);

  huffmanEncode(input, compressed);
  huffmanDecode(compressed, decompressed);

  bool isCorrect = filesAreEqual(input, decompressed);
  printf("Encoded %s %s\n", input, isCorrect ? "correctly" : "incorrectly");
}

void testHuffman() {
  testHuffmanEntry("book.txt");
}

int main() {
  // testSparceIndex();
  testHuffman();

  return 0;
}
