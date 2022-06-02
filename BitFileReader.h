#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#pragma once

typedef struct {
  FILE* file;
  unsigned char curByte;
  unsigned char bitsReadInByte;
} BitFileReader;

BitFileReader* createBitFileReader(FILE* file) {
  BitFileReader* result = malloc(sizeof(BitFileReader));
  result->file = file;

  fread(&result->curByte, 1, 1, file);

  result->bitsReadInByte = 0;

  return result;
}

unsigned char readBit(BitFileReader* reader) {
  unsigned char result = (reader->curByte & (1 << (7 - reader->bitsReadInByte))) != 0;

  reader->bitsReadInByte += 1;
  if (reader->bitsReadInByte == 8) {
    reader->bitsReadInByte = 0;
    fread(&reader->curByte, 1, 1, reader->file);
  }

  return result;
}

// void writeBits(BitFileWriter* writer, unsigned char amount, unsigned char* source) {
//   for (int i = 0; i < amount; i += 1) {
//     writer->curByte <<= 1;
//     writer->curByte |= source[i];

//     writer->bitsWrittenInByte += 1;

//     if (writer->bitsWrittenInByte == 8) {
//       printf("BITS "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(writer->curByte));
//       fwrite(&writer->curByte, 1, 1, writer->file);
//       writer->curByte = 0;
//       writer->bitsWrittenInByte = 0;
//     }
//   }
// }

// void padWithZeroes(BitFileWriter* writer) {
//   if (writer->bitsWrittenInByte == 0) {
//     return;
//   }

//   writer->curByte << (8 - writer->bitsWrittenInByte);
//   fwrite(&writer->curByte, 1, 1, writer->file);
//   writer->curByte = 0;
//   writer->bitsWrittenInByte = 0;
// }
