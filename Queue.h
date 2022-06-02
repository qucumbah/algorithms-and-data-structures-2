#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma once

typedef struct {
  size_t itemSize;
  int capacity;
  int nextIdx;
  void* items;
} Queue;

Queue* createQueueFromItems(void* items, size_t itemSize, int capacity) {
  assert(capacity > 0);

  Queue* result = malloc(sizeof(Queue));

  result->itemSize = itemSize;
  result->capacity = capacity;
  result->nextIdx = capacity;
  result->items = items;

  return result;
}

Queue* createQueue(size_t itemSize, int capacity) {
  assert(capacity > 0);

  Queue* result = malloc(sizeof(Queue));

  result->itemSize = itemSize;
  result->capacity = capacity;
  result->nextIdx = 0;
  result->items = malloc(capacity * itemSize);

  return result;
}

void* getFront(Queue* queue) {
  assert(queue->nextIdx != 0);

  return queue->items;
}

void* getBack(Queue* queue) {
  assert(queue->nextIdx != 0);

  return queue->items + (queue->nextIdx - 1) * queue->itemSize;
}

void rmFront(Queue* queue) {
  assert(queue->nextIdx != 0);

  memmove(queue->items, queue->items + queue->itemSize, queue->nextIdx * queue->itemSize);
  queue->nextIdx -= 1;
}

void addBack(Queue* queue, void* item) {
  assert(queue->nextIdx != queue->capacity);

  memmove(queue->items + queue->nextIdx * queue->itemSize, item, queue->itemSize);
  queue->nextIdx += 1;
}
