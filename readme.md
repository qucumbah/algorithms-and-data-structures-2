# Algorithms and data structures written in pure C

This is a collection of various algorithms and data structures that I wrote during my ADS course in university.

- [Algorithms and data structures written in pure C](#algorithms-and-data-structures-written-in-pure-c)
  - [Huffman encoding](#huffman-encoding)
    - [Encoding and decoding](#encoding-and-decoding)
    - [Graphical user interface for Huffman algorithm](#graphical-user-interface-for-huffman-algorithm)
  - [Utility](#utility)
    - [Swap](#swap)
    - [Comparator](#comparator)
    - [Mapping array](#mapping-array)
    - [Binary search](#binary-search)
    - [File equality](#file-equality)
    - [File size](#file-size)
    - [Bit file manipulation](#bit-file-manipulation)
  - [Sorting functions](#sorting-functions)
    - [Quicksort](#quicksort)
    - [Heapsort](#heapsort)
    - [File merge sort](#file-merge-sort)
  - [Data structures](#data-structures)
    - [Dynamically resizing list](#dynamically-resizing-list)
    - [Queue](#queue)
    - [Hash table](#hash-table)
      - [Hashing functions](#hashing-functions)
  - [Sparce index search](#sparce-index-search)
    - [Block file](#block-file)
    - [Index file](#index-file)
    - [Sparce index manipulation](#sparce-index-manipulation)

## Huffman encoding

### Encoding and decoding

Main file: [`huffman.h`](./huffman.h).

```c
void huffmanEncode(char* inFilePath, char* outFilePath);
void huffmanDecode(char* inFilePath, char* outFilePath);
```

Encodes or decode a file using [Huffman algorithm](https://en.wikipedia.org/wiki/Huffman_coding).
Uses a special technique for saving code table.
Instead of storing frequencies of each symbol, it stores the resolved frequency tree, which allows the header to be a bit shorter.

### Graphical user interface for Huffman algorithm

Main file: [`huffmanGui.h`](./huffmanGui.h).

![image](https://user-images.githubusercontent.com/39967396/174433884-228273cd-3f86-40ee-bd46-0505c6da5529.png)

Graphical interface written in GTK+ for simplifying Huffman algorithm testing and performance measurement.

## Utility

Main file: [`util.h`](./util.h).

This file includes a few common structs and functions that are used throughout the codebase.

### Swap

```c
void swap(void* array, size_t itemSize, int a, int b);
```

Swaps two elements with indices `a` and `b` in `array` that has elements of size `itemSize`.

### Comparator

```c
typedef int (Comparator)(const void* a, const void* b);
```

Type definitions for comparison function that is used in sorting and search algorithms.

### Mapping array

```c
typedef void (MapFn)(void* from, void* to);

void* mapArray(
  void* from,
  size_t fromSize,
  size_t toSize,
  int arrayLength,
  MapFn mapFn
);
```

Function for mapping an array `from` of items of size `fromSize` with length `arrayLength` into a new array with items of size `toSize` with provided mapping function `mapFn`.

### Binary search

```c
int binSearch(
  void* array,
  size_t itemSize,
  int arrayLength,
  void* itemToFind,
  Comparator comparator
)
```

Function for searching for item `itemToFind` in a sorted array `array` with items of size `itemSize` and length `arrayLength`. Comparison between elements is done via a provided `comparator` function.

### File equality

```c
bool filesAreEqual(char* path1, char* path2);
```

Checks if files located at `path1` and `path2` are equal byte by byte.

### File size

```c
int fileSize(char* path);
```

Returns the size of the file located at `path` in bytes.

### Bit file manipulation

Main files: [`BitFileReader.h`](./BitFileReader.h), [`BitFileWriter.h`](./BitFileWriter.h).

```c
typedef struct {
  FILE* file;
  unsigned char curByte;
  unsigned char bitsReadInByte;
} BitFileReader;

BitFileReader* createBitFileReader(FILE* file);
unsigned char readBit(BitFileReader* reader);

typedef struct {
  FILE* file;
  unsigned char curByte;
  unsigned char bitsWrittenInByte;
} BitFileWriter;

BitFileWriter* createBitFileWriter(FILE* file);
void writeBits(BitFileWriter* writer, unsigned char amount, unsigned char* source);
void padWithZeroes(BitFileWriter* writer);
```

Allows to read and write data to file bit-by-bit.

## Sorting functions

Includes internal (RAM) and external (file system) data sorting.

### Quicksort

Main file: [`quicksort.h`](./quicksort.h).

```c
void quicksort(
  void* array,
  size_t itemSize,
  int arrayLength,
  Comparator* comparator
);
```

A function for qsorting a generic array `array` with items of size `itemSize` and length `arrayLength`. Uses the provided `comparator` function for comparing array items.

### Heapsort

Main file: [`heapsort.h`](./heapsort.h).

```c
void heapsort(
  void* array,
  size_t itemSize,
  int arrayLength,
  Comparator* comparator
);
```

A function for heap sorting a generic array `array` with items of size `itemSize` and length `arrayLength`. Uses the provided `comparator` function for comparing array items.

### File merge sort

Main file: [`mergesort.h`](./mergesort.h).

```c
void mergesort(
  char* file,
  size_t itemSize,
  int length,
  Comparator* comparator
);
```

A function for two-phase merge sorting an array of length `length` and items of size `itemSize` stored at location `file` of the file system. Uses the provided `comparator` function for comparing array items.
This merge sorting method uses two helper buffer files to optimize RAM consumption.

## Data structures

### Dynamically resizing list

Main file: [`List.h`](./List.h).

```c
typedef struct {
  size_t itemSize;
  int capacity;
  int size;
  void* items;
} List;

List* createList(size_t itemSize);
void listAdd(List* list, void* value);
void* listGet(List* list, int index);
int listFind(List* list, void* valueToFind, Comparator comparator);
void listDestroy(List* list);
```

List that stores any items of size `itemSize`.
Supports looking for an element `valueToFind` with predicate function `comparator`.

### Queue

Main file: [`Queue.h`](./Queue.h).

```c
typedef struct {
  size_t itemSize;
  int capacity;
  int nextIdx;
  void* items;
} Queue;

Queue* createQueueFromItems(void* items, size_t itemSize, int capacity);
Queue* createQueue(size_t itemSize, int capacity);
void* getFront(Queue* queue);
void* getBack(Queue* queue);
void rmFront(Queue* queue);
void rmBack(Queue* queue);
void addFront(Queue* queue, void* item);
void addBack(Queue* queue, void* item);
int queueSize(Queue* queue);
void destroyQueue(Queue* queue);
```

A fixed `capacity` queue storing any `items` of size `itemSize`.

### Hash table

Main file: [`HashTable.h`](./HashTable.h).

```c
typedef struct {
  size_t itemSize;
  int capacity;
  int size;
  double alpha;
  List** items;
} HashTable;

typedef struct {
  char* key;
  void* item;
} HashTableEntry;

HashTable* createHashTable(size_t itemSize);
void hashTableAddEntry(HashTable* table, HashTableEntry* entry);
void hashTableAdd(HashTable* table, char* key, void* item);
void* hashTableGet(HashTable* table, char* key);
```

Dynamically-sized hash table with list-based collision resolution method.

#### Hashing functions

Main file: [`hash.h`](./hash.h).

```c
int prehash(char* key);
int hash(char* key, int tableCapacity);
```

Functions for getting a prehash or a hash for a zero-terminated string. Uses prehash calculation based on circular shift and addition.

## Sparce index search

Sparce index search allows to store data of arbitrary size in multiple block files of varying length. Index file stores which block the data is stored in.

### Block file

Main file: [`blockFile.h`](./blockFile.h).

```c
typedef struct {
  size_t size;
  void* data;
} BlockFileEntry;

BlockFileEntry readFromBlock(char* blockFilePath, size_t key);
void writeToBlock(char* blockFilePath, size_t key, void* value, size_t size);
void createBlockFile(char* path);
```

Allows to initialize, read from and write to block files.
Data descriptors are sorted, so reading takes O(log n).
Writing and initialization is O(1).

### Index file

Main file: [`indexFile.h`](./indexFile.h).

```c
typedef struct {
  size_t start;
  size_t end;
} BlockSizeDescriptor;

typedef struct {
  BlockSizeDescriptor sizeDescriptor;
  int number;
} Block;

Block getBlockForKey(char* indexFilePath, size_t key);
void createIndexFile(char* location);
void expandIndexFile(char* indexFilePath, size_t size);
```

Allows to create and expand the index file that is used to look for block file where the data is stored.

### Sparce index manipulation

Main file: [`sparceIndex.h`](./sparceIndex.h).

```c
void sparceIndexWrite(size_t key, void* value, size_t size);
BlockFileEntry sparceIndexRead(size_t key);
void sparceIndexInit();
void sparceIndexExpand(size_t size);
```

Allows to write and read arbitrary data using a sparce index.
