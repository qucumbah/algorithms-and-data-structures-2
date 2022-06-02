#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef int (Comparator)(const void* a, const void* b);

void checkSize(FILE* file) {
  fseek(file, 0, SEEK_END);
  printf("%ld\n", ftell(file));
  rewind(file);
}

void split(char* file, size_t itemSize, int s) {
  FILE* a = fopen(file, "rb");
  FILE* b1 = fopen("buffer1.bin", "wb");
  FILE* b2 = fopen("buffer2.bin", "wb");

  char* itemBytes = malloc(itemSize);
  fread(itemBytes, itemSize, 1, a);

  while (!feof(a)) {
    for (int j = 0; j < s && !feof(a); j += 1) {
      fwrite(itemBytes, itemSize, 1, b1);
      fread(itemBytes, itemSize, 1, a);
    }
    for (int j = 0; j < s && !feof(a); j += 1) {
      fwrite(itemBytes, itemSize, 1, b2);
      fread(itemBytes, itemSize, 1, a);
    }
  }
  free(itemBytes);

  fclose(b1);
  fclose(b2);
}

void merge(char* file, size_t itemSize, int length, int s, Comparator* comparator) {
  FILE* a = fopen(file, "wb");
  FILE* b1 = fopen("buffer1.bin", "rb");
  FILE* b2 = fopen("buffer2.bin", "rb");

  char* itemBytes1 = malloc(itemSize);
  char* itemBytes2 = malloc(itemSize);

  fread(itemBytes1, itemSize, 1, b1);
  fread(itemBytes2, itemSize, 1, b2);

  while (!feof(b1) && !feof(b2)) {
    int i1 = 0;
    int i2 = 0;

    while (i1 < s && i2 < s && !feof(b1) && !feof(b2)) {
      if ((*comparator)((const void*)itemBytes1, (const void*)itemBytes2) < 0) {
        fwrite(itemBytes1, itemSize, 1, a);
        fread(itemBytes1, itemSize, 1, b1);
        i1 += 1;
      } else {
        fwrite(itemBytes2, itemSize, 1, a);
        fread(itemBytes2, itemSize, 1, b2);
        i2 += 1;
      }
    }

    while (i1 < s && !feof(b1)) {
      fwrite(itemBytes1, itemSize, 1, a);
      fread(itemBytes1, itemSize, 1, b1);
      i1 += 1;
    }

    while (i2 < s && !feof(b2)) {
      fwrite(itemBytes2, itemSize, 1, a);
      fread(itemBytes2, itemSize, 1, b2);
      i2 += 1;
    }
  }

  while (!feof(b1)) {
    fwrite(itemBytes1, itemSize, 1, a);
    fread(itemBytes1, itemSize, 1, b1);
  }

  while (!feof(b2)) {
    fwrite(itemBytes2, itemSize, 1, a);
    fread(itemBytes2, itemSize, 1, b2);
  }

  free(itemBytes1);
  free(itemBytes2);

  fclose(a);
  fclose(b1);
  fclose(b2);
}

void mergesort(char* file, size_t itemSize, int length, Comparator* comparator) {
  int s = 1;
  while (s < length) {
    // Фаза 1
    split(file, itemSize, s);
    // фаза 2
    merge(file, itemSize, length, s, comparator);
    s *= 2;
  }
}

int compareInts(int* a, int* b) {
  return *a - *b;
}

int main() {
  int array[15] = {4, 22, 65, 67, 77, 87, 90, 23, 76, 90, 74, 5, 74, 89, 82};

  FILE* arrayStorage = fopen("array.bin", "wb");
  fwrite(array, sizeof(int), sizeof(array) / sizeof(int), arrayStorage);
  fclose(arrayStorage);
  // system("hexdump ./array.bin");

  mergesort("array.bin", sizeof(int), sizeof(array) / sizeof(int), compareInts);
  // system("hexdump ./array.bin");

  return 0;
}
