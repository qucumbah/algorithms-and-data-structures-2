#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"
#include "quicksort.h"
#include "BitFileWriter.h"
#include "BitFileReader.h"

#pragma once

#if DEBUG != 1
  #define printf printfDummy
#endif

int* calculateFrequencies(FILE* file) {
  int* result = calloc(256, sizeof(int));

  unsigned char byteBuffer;
  fread(&byteBuffer, 1, 1, file);

  while (!feof(file)) {
    result[byteBuffer] += 1;
    fread(&byteBuffer, 1, 1, file);
  }

  return result;
}

int getNonZeroFrequenciesCount(int* frequencies) {
  int nonZeroCount = 0;

  for (int i = 0; i < 256; i += 1) {
    if (frequencies[i] != 0) {
      nonZeroCount += 1;
    }
  }

  return nonZeroCount;
}

int* getNonZeroFrequencies(int* frequencies) {
  int nonZeroCount = getNonZeroFrequenciesCount(frequencies);

  Queue* nonZeroes = createQueue(sizeof(int), nonZeroCount);
  for (int i = 0; i < 256; i += 1) {
    if (frequencies[i] != 0) {
      addBack(nonZeroes, &frequencies[i]);
    }
  }

  assert(nonZeroes->capacity == nonZeroes->nextIdx);

  return nonZeroes->items;
}

struct Tree {
  int frequency;
  bool isLeaf;

  struct Tree* left;
  struct Tree* right;

  char symbol;
};

typedef struct Tree Tree;

Tree* createNodeFromFrequency(char symbol, int frequency) {
  Tree* result = malloc(sizeof(Tree));
  
  result->frequency = frequency;
  result->isLeaf = true;
  result->symbol = symbol;

  return result;
}

int compareNodes(const void* a, const void* b) {
  return ((Tree*)a)->frequency - ((Tree*)b)->frequency;
}

Tree* getLowestNode(Queue* q1, Queue* q2) {
  Tree* lowest = malloc(sizeof(Tree));

  if (queueSize(q1) == 0) {
    *lowest = *(Tree*)getFront(q2);
    rmFront(q2);
  } else if (queueSize(q2) == 0) {
    *lowest = *(Tree*)getFront(q1);
    rmFront(q1);
  } else {
    if (compareNodes(getFront(q1), getFront(q2)) < 0) {
      *lowest = *(Tree*)getFront(q1);
      rmFront(q1);
    } else {
      *lowest = *(Tree*)getFront(q2);
      rmFront(q2);
    }
  }

  return lowest;
}

Tree* buildTree(Tree* nodes, int nonZeroFrequenciesCount) {
  Queue* q1 = createQueueFromItems(nodes, sizeof(Tree), nonZeroFrequenciesCount);
  Queue* q2 = createQueue(sizeof(Tree), nonZeroFrequenciesCount);

  while (queueSize(q1) != 0 || queueSize(q2) != 1) {
    Tree* lowest1 = getLowestNode(q1, q2);
    Tree* lowest2 = getLowestNode(q1, q2);

    Tree newNode = {
      .frequency = lowest1->frequency + lowest2->frequency,
      .isLeaf = false,
      .left = lowest1,
      .right = lowest2,
    };

    addBack(q2, &newNode);
  }

  assert(queueSize(q2) == 1);

  Tree* result = malloc(sizeof(Tree));
  *result = *(Tree*)getFront(q2);

  destroyQueue(q1);
  destroyQueue(q2);

  return result;
}

void logTree(Tree* tree, int code) {
  if (!tree->isLeaf) {
    printf("going left\n");
    logTree(tree->left, code << 1 | 1);
    printf("going right\n");
    logTree(tree->right, code << 1 | 0);
    printf("going up\n");
    return;
  }

  printf("%c "BYTE_TO_BINARY_PATTERN"\n", tree->symbol, BYTE_TO_BINARY(code));
  printf("going up\n");
}

void traverseTreeBuildTable(
  Tree* tree,
  unsigned char* codeLengths,
  unsigned char** codes,
  unsigned char* currentCode,
  int currentCodeNextBitIndex
) {
  if (tree->isLeaf) {
    unsigned char c = (unsigned char)tree->symbol;
    int codeLength = currentCodeNextBitIndex;
    codeLengths[c] = codeLength;
    codes[c] = malloc(codeLength);
    memmove(codes[c], currentCode, codeLength);
    printf("should be same %d %d\n", currentCode[1], codes[c][1]);
    return;
  }

  currentCode[currentCodeNextBitIndex] = 0;
  traverseTreeBuildTable(
    tree->left,
    codeLengths,
    codes,
    currentCode,
    currentCodeNextBitIndex + 1
  );

  currentCode[currentCodeNextBitIndex] = 1;
  traverseTreeBuildTable(
    tree->right,
    codeLengths,
    codes,
    currentCode,
    currentCodeNextBitIndex + 1
  );
}

void buildCodesTable(Tree* tree, unsigned char* codeLengths, unsigned char** codes) {
  unsigned char* currentCode = malloc(255);

  traverseTreeBuildTable(
    tree,
    codeLengths,
    codes,
    currentCode,
    0
  );
}

void logTable(unsigned char* codeLengths, unsigned char** codes) {
  for (int i = 0; i < 256; i += 1) {
    if (codeLengths[i] == 0) {
      continue;
    }

    printf("%c ", i);
    for (int j = 0; j < codeLengths[i]; j += 1) {
      printf("%d", codes[i][j]);
    }
    printf("\n");
  }
}

void writeTree(
  FILE* outFile,
  Tree* tree
) {
  unsigned char isLeaf = tree->isLeaf;
  fwrite(&isLeaf, 1, 1, outFile);

  if (isLeaf) {
    fwrite(&tree->symbol, 1, 1, outFile);
    return;
  }

  unsigned char hasLeft = tree->left != NULL;
  unsigned char hasRight = tree->right != NULL;
  fwrite(&hasLeft, 1, 1, outFile);
  fwrite(&hasRight, 1, 1, outFile);

  printf("tree %d %d %d %ld %ld\n", isLeaf, hasLeft, hasRight, tree->left, tree->right);

  if (hasLeft) {
    writeTree(outFile, tree->left);
  }

  if (hasRight) {
    writeTree(outFile, tree->right);
  }
}

/*
Format:
tree nodes:
  byte isleaf
  for leaves:
    byte symbol
  for non-leaves:
    byte hasLeft
    byte hasRight
total message length - the end of the header
message bits
*/
void writeHeader(
  FILE* outFile,
  int* frequencies,
  int nonZeroFrequenciesCount,
  unsigned char* codeLengths,
  Tree* tree
) {
  writeTree(outFile, tree);

  unsigned long totalMessageLength = 0;
  for (int i = 0; i < 256; i += 1) {
    totalMessageLength += frequencies[i] * codeLengths[i];
  }

  fwrite(&totalMessageLength, sizeof(unsigned long), 1, outFile);
}

void writeMessage(FILE* outFile, unsigned char* codeLengths, unsigned char** codes, FILE* inFile) {
  BitFileWriter* writer = createBitFileWriter(outFile);

  unsigned char c;
  fread(&c, 1, 1, inFile);

  while (!feof(inFile)) {
    unsigned char codeLength = codeLengths[c];
    unsigned char* code = codes[c];

    writeBits(writer, codeLength, code);

    fread(&c, 1, 1, inFile);
  }

  padWithZeroes(writer);
}

Tree* huffmanEncode(char* inFilePath, char* outFilePath) {
  FILE* inFile = fopen(inFilePath, "rb");

  int* frequencies = calculateFrequencies(inFile);

  int nonZeroFrequenciesCount = getNonZeroFrequenciesCount(frequencies);

  Tree* nodes = malloc(sizeof(Tree) * nonZeroFrequenciesCount);
  int nonZeroIndex = 0;
  for (int i = 0; i < 256; i += 1) {
    if (frequencies[i] == 0) {
      continue;
    }
    Tree* nonZeroNode = createNodeFromFrequency(i, frequencies[i]);
    nodes[nonZeroIndex] = *nonZeroNode;
    free(nonZeroNode);
    nonZeroIndex += 1;
  }

  for (int i = 0; i < nonZeroFrequenciesCount; i += 1) {
    printf("%d %d\n", nodes[i].symbol, nodes[i].frequency);
  }

  printf("---\n");

  quicksort(nodes, sizeof(Tree), nonZeroFrequenciesCount, compareNodes);

  for (int i = 0; i < nonZeroFrequenciesCount; i += 1) {
    printf("%d %d\n", nodes[i].symbol, nodes[i].frequency);
  }

  Tree* tree = buildTree(nodes, nonZeroFrequenciesCount);

  unsigned char codeLengths[256] = { 0 };
  unsigned char* codes[256] = { NULL };
  buildCodesTable(tree, codeLengths, codes);

  logTree(tree, 1);

  logTable(codeLengths, codes);

  FILE* outFile = fopen(outFilePath, "wb");

  assert(inFile != outFile);

  writeHeader(outFile, frequencies, nonZeroFrequenciesCount, codeLengths, tree);
  free(frequencies);

  fclose(inFile);

  inFile = fopen(inFilePath, "rb");

  writeMessage(outFile, codeLengths, codes, inFile);

  fclose(outFile);

  return tree;
}

Tree* readTree(FILE* inFile) {
  unsigned char isLeaf;
  fread(&isLeaf, 1, 1, inFile);

  if (isLeaf) {
    unsigned char c;
    fread(&c, 1, 1, inFile);

    Tree* result = malloc(sizeof(Tree));
    result->isLeaf = true;
    result->symbol = c;

    return result;
  }

  unsigned char hasLeft;
  unsigned char hasRight;
  fread(&hasLeft, 1, 1, inFile);
  fread(&hasRight, 1, 1, inFile);

  Tree* left;
  Tree* right;

  if (hasLeft) {
    left = readTree(inFile);
  }
  if (hasRight) {
    right = readTree(inFile);
  }

  Tree* result = malloc(sizeof(Tree));
  result->isLeaf = false;
  result->left = left;
  result->right = right;

  return result;
}

void huffmanDecode(char* inFilePath, char* outFilePath) {
  FILE* inFile = fopen(inFilePath, "rb");
  Tree* tree = readTree(inFile);
  logTree(tree, 1);

  unsigned long bits;
  fread(&bits, sizeof(unsigned long), 1, inFile);

  printf("%d\n", bits);

  FILE* outFile = fopen(outFilePath, "wb");

  BitFileReader* reader = createBitFileReader(inFile);

  Tree* curTree = tree;
  for (unsigned long i = 0; i < bits; i += 1) {
    unsigned char bit = readBit(reader);
    printf("%d %d\n", i, bit);

    if (bit == 0) {
      curTree = curTree->left;
    } else {
      curTree = curTree->right;
    }

    if (curTree->isLeaf) {
      printf("found symbol %c\n", curTree->symbol);
      fwrite(&curTree->symbol, 1, 1, outFile);
      curTree = tree;
    }
  }

  fclose(outFile);
}

#if DEBUG != 1
  #undef printf
#endif
