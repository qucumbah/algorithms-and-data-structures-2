#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#pragma once

typedef struct {
  size_t key;
  size_t start;
  size_t size;
} BlockFileDataDescriptor;

int compareEntries(const void* a, const void* b) {
  BlockFileDataDescriptor* entryA = (BlockFileDataDescriptor*)a;
  BlockFileDataDescriptor* entryB = (BlockFileDataDescriptor*)b;
  
  // Keys are unsigned, have to return comparison result manually
  if (entryA->key == entryB->key) {
    return 0;
  }

  return entryA->key < entryB->key ? -1 : 1;
}

/*
Block file structure:
int amount of entries N
(3 * size_t) * N sorted data descriptors
byte[] data that descriptors point to

Entry structure:
size_t key
size_t start
size_t size
*/
void* readFromBlock(char* blockFilePath, size_t key) {
  FILE* blockFile = fopen(blockFilePath, "rb");

  int entriesCount;
  fread(&entriesCount, sizeof(int), 1, blockFile);

  BlockFileDataDescriptor* entries = malloc(entriesCount * sizeof(BlockFileDataDescriptor));
  fread(entries, sizeof(BlockFileDataDescriptor), entriesCount, blockFile);

  BlockFileDataDescriptor entryToFind = {
    .key = key,
  };

  int foundEntryIndex = binSearch(
    entries,
    sizeof(BlockFileDataDescriptor),
    entriesCount,
    &entryToFind,
    compareEntries
  );

  BlockFileDataDescriptor* foundEntry = &entries[foundEntryIndex];

  if (foundEntry->key != key) {
    return NULL;
  }

  size_t start = foundEntry->start;
  size_t size = foundEntry->size;

  free(entries);

  fseek(blockFile, start, SEEK_SET);

  void* result = malloc(size);
  fread(result, size, 1, blockFile);

  fclose(blockFile);

  return result;
}

void writeToBlock(char* blockFilePath, size_t key, void* value, size_t size) {
  assert(size != 0);

  FILE* blockFile = fopen(blockFilePath, "rb");

  int entriesCount;
  fread(&entriesCount, sizeof(int), 1, blockFile);

  BlockFileDataDescriptor* entries = malloc((entriesCount + 1) * sizeof(BlockFileDataDescriptor));
  fread(entries, sizeof(BlockFileDataDescriptor), entriesCount, blockFile);

  BlockFileDataDescriptor entryToFind = {
    .key = key,
  };

  int foundEntryIndex = (entriesCount == 0) ? 0 : binSearch(
    entries,
    sizeof(BlockFileDataDescriptor),
    entriesCount,
    &entryToFind,
    compareEntries
  );

  BlockFileDataDescriptor* foundEntry = &entries[foundEntryIndex];

  assert(foundEntryIndex == entriesCount || foundEntry->key != key);

  entriesCount += 1;
  for (int i = entriesCount - 1; i > foundEntryIndex; i -= 1) {
    entries[i] = entries[i - 1];
  }

  // If we don't have any elements, we only have one int (0 for the amount of entries) in the file,
  // so we need to start writing data after that int.
  // If we have at least one element, write the data right after it.
  // Entries count was increased due to addition of the new element, so we check if it's equal to 1.
  size_t lastValueStart = (entriesCount == 1) ? sizeof(int) : 0;
  size_t lastValueSize = 0;

  for (int i = 0; i < entriesCount; i += 1) {
    if (i == foundEntryIndex) {
      continue;
    }

    if (entries[i].start > lastValueStart) {
      lastValueStart = entries[i].start;
      lastValueSize = entries[i].size;
    }
  }

  entries[foundEntryIndex].key = key;
  entries[foundEntryIndex].start = lastValueStart + lastValueSize;
  entries[foundEntryIndex].size = size;

  // Adding a new entry shifts all the data
  for (int i = 0; i < entriesCount; i += 1) {
    entries[i].start += sizeof(BlockFileDataDescriptor);
  }

  char tempFilePath[] = "temp.bin";
  FILE* tempFile = fopen(tempFilePath, "wb");
  fwrite(&entriesCount, sizeof(int), 1, tempFile);
  fwrite(entries, sizeof(BlockFileDataDescriptor), entriesCount, tempFile);

  char byte;
  fread(&byte, 1, 1, blockFile);
  while (!feof(blockFile)) {
    fwrite(&byte, 1, 1, tempFile);
    fread(&byte, 1, 1, blockFile);
  }

  fclose(blockFile);

  fwrite(value, size, 1, tempFile);

  fclose(tempFile);

  rename(tempFilePath, blockFilePath);
}

void createBlockFile(char* path) {
  FILE* blockFile = fopen(path, "wb");
  int initialAmountOfEntries = 0;
  fwrite(&initialAmountOfEntries, sizeof(int), 1, blockFile);
  fclose(blockFile);
}
