#include <stdlib.h>

#include "indexFile.h"
#include "blockFile.h"

void sparceIndexWrite(size_t key, void* value, size_t size) {
  Block blockFile = getBlockForKey("./data/index.bin", key);

  char blockFilePath[256];
  sprintf(blockFilePath, "./data/block%d.bin", blockFile.number);

  FILE* blockFileHandle = fopen(blockFilePath, "r");
  if (blockFileHandle == NULL) {
    createBlockFile(blockFilePath);
  } else {
    fclose(blockFileHandle);
  }

  writeToBlock(blockFilePath, key - blockFile.sizeDescriptor.start, value, size);
}

BlockFileEntry sparceIndexRead(size_t key) {
  Block blockFile = getBlockForKey("./data/index.bin", key);

  char blockFilePath[256];
  sprintf(blockFilePath, "./data/block%d.bin", blockFile.number);

  FILE* blockFileHandle = fopen(blockFilePath, "r");
  if (blockFileHandle == NULL) {
    createBlockFile(blockFilePath);
  } else {
    fclose(blockFileHandle);
  }

  return readFromBlock(blockFilePath, key - blockFile.sizeDescriptor.start);
}

void sparceIndexInit() {
  createIndexFile("./data/index.bin");
}

void sparceIndexExpand(size_t size) {
  expandIndexFile("./data/index.bin", size);
}
