#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"

#pragma once

typedef struct {
  size_t itemSize;
  int capacity;
  int size;
  void* items;
} List;

List* createList(size_t itemSize) {
  assert(itemSize > 0);

  List* result = malloc(sizeof(List));

  result->itemSize = itemSize;

  int initialCapacity = 16;
  result->capacity = initialCapacity;
  result->items = calloc(initialCapacity, itemSize);

  result->size = 0;

  return result;
}

void listResizeIfNeeded(List* list, int newSize) {
  if (newSize <= list->capacity) {
    return;
  }

  int newCapacity = list->capacity * 2;
  void* newItems = calloc(newCapacity, list->itemSize);

  memmove(newItems, list->items, list->size * list->itemSize);

  free(list->items);
  list->items = newItems;
  list->capacity = newCapacity;
}

void listAdd(List* list, void* value) {
  listResizeIfNeeded(list, list->size + 1);
  memmove(list->items + list->size * list->itemSize, value, list->itemSize);
  list->size += 1;
}

void* listGet(List* list, int index) {
  return list->items + index * list->itemSize;
}

int listFind(List* list, void* valueToFind, Comparator comparator) {
  for (int i = 0; i < list->size; i += 1) {
    void* curValue = list->items + i * list->itemSize;

    if (comparator(curValue, valueToFind) == 0) {
      return i;
    }
  }

  return -1;
}

void listDestroy(List* list) {
  free(list->items);
  free(list);
}
