#include <stdlib.h>
#include <stdio.h>

#include "util.h"

#pragma once

/*
Index file structure:
int amount of blocks N
sizeof(BlockSizeDescriptor) * N block descriptors
*/

typedef struct {
  size_t start;
  size_t end;
} BlockSizeDescriptor;

// Block B is the search item, where start==end==key
int findBlock(const void* a, const void* b) {
  const BlockSizeDescriptor* blockA = a;
  const BlockSizeDescriptor* blockB = b;

  if (blockB->start >= blockA->start && blockB->start < blockA->end) {
    return 0;
  }

  return blockA->start < blockB->start ? -1 : 1;
}

typedef struct {
  BlockSizeDescriptor sizeDescriptor;
  int number;
} Block;

Block getBlockForKey(char* indexFilePath, size_t key) {
  FILE* indexFile = fopen(indexFilePath, "rb");

  int amountOfBlocks;
  fread(&amountOfBlocks, sizeof(int), 1, indexFile);

  BlockSizeDescriptor* blocks = malloc(sizeof(BlockSizeDescriptor) * amountOfBlocks);
  fread(blocks, sizeof(BlockSizeDescriptor), amountOfBlocks, indexFile);

  assert(key < blocks[amountOfBlocks - 1].end);

  BlockSizeDescriptor searchItem = {
    .start = key,
    .end = key,
  };

  int foundIndex = binSearch(
    blocks,
    sizeof(BlockSizeDescriptor),
    amountOfBlocks,
    &searchItem,
    findBlock
  );
  BlockSizeDescriptor* desiredBlock = &blocks[foundIndex];

  Block result = {
    .sizeDescriptor = *desiredBlock,
    .number = foundIndex,
  };

  fclose(indexFile);

  return result;
}

void createIndexFile(char* location) {
  FILE* indexFile = fopen(location, "wb");
  int initialAmountOfEntries = 0;
  fwrite(&initialAmountOfEntries, 1, sizeof(int), indexFile);
  fclose(indexFile);
}

void expandIndexFile(char* indexFilePath, size_t size) {
  FILE* indexFile = fopen(indexFilePath, "rb");

  int amountOfBlocks;
  fread(&amountOfBlocks, sizeof(int), 1, indexFile);

  BlockSizeDescriptor* blocks = malloc(sizeof(BlockSizeDescriptor) * (amountOfBlocks + 1));
  fread(blocks, sizeof(BlockSizeDescriptor), amountOfBlocks, indexFile);

  fclose(indexFile);

  amountOfBlocks += 1;
  blocks[amountOfBlocks - 1].start = (amountOfBlocks == 1) ? 0 : blocks[amountOfBlocks - 2].end;
  blocks[amountOfBlocks - 1].end = blocks[amountOfBlocks - 1].start + size;

  indexFile = fopen(indexFilePath, "wb");
  fwrite(&amountOfBlocks, sizeof(int), 1, indexFile);
  fwrite(blocks, sizeof(BlockSizeDescriptor), amountOfBlocks, indexFile);
  fclose(indexFile);
}
