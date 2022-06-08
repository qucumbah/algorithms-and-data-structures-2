#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "List.h"
#include "hash.h"

#pragma once

typedef struct {
  char* key;
  void* item;
} HashTableEntry;

typedef struct {
  size_t itemSize;
  int capacity;
  int size;
  double alpha;
  List** items;
} HashTable;

HashTable* createHashTable(size_t itemSize) {
  HashTable* result = malloc(sizeof(HashTable));

  result->itemSize = itemSize;

  result->alpha = 0.5;

  int initialCapacity = 16;
  result->capacity = initialCapacity;
  result->items = calloc(initialCapacity, sizeof(List*));

  result->size = 0;

  return result;
}

void hashTableResizeIfNeeded(HashTable* table, size_t newSize);

void hashTableAddEntry(HashTable* table, HashTableEntry* entry) {
  int hashValue = hash(entry->key, table->capacity);

  if (table->items[hashValue] == NULL) {
    List* collisionResolutionList = createList(sizeof(HashTableEntry));
    table->items[hashValue] = collisionResolutionList;
  }

  listAdd(table->items[hashValue], entry);
}

void hashTableAdd(HashTable* table, char* key, void* item) {
  void* itemCopy = malloc(table->itemSize);
  memmove(itemCopy, item, table->itemSize);

  HashTableEntry entry = {
    .key = key,
    .item = itemCopy,
  };
  hashTableAddEntry(table, &entry);
}

void* hashTableGet(HashTable* table, char* key) {
  int hashValue = hash(key, table->capacity);

  List* collisionResolutionList = table->items[hashValue];

  if (collisionResolutionList == NULL) {
    return NULL;
  }

  for (int i = 0; i < collisionResolutionList->size; i += 1) {
    HashTableEntry* entry = listGet(collisionResolutionList, i);
    if (strcmp(key, entry->key) == 0) {
      return entry->item;
    }
  }
}

void hashTableResizeIfNeeded(HashTable* table, size_t newSize) {
  if (newSize <= table->capacity * table->alpha) {
    return;
  }

  int oldCapacity = table->capacity;
  List** oldItems = table->items;

  table->capacity *= 2;
  table->items = calloc(table->capacity, sizeof(List*));
  table->size = 0;

  for (int i = 0; i < oldCapacity; i += 1) {
    if (oldItems[i] == NULL) {
      continue;
    }

    for (int j = 0; j < oldItems[i]->size; j += 1) {
      hashTableAddEntry(table, listGet(oldItems[i], j));
    }

    listDestroy(oldItems[i]);
  }
}
