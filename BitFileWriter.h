#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#pragma once

typedef struct {
  FILE* file;
  unsigned char curByte;
  unsigned char bitsWrittenInByte;
} BitFileWriter;

BitFileWriter* createBitFileWriter(FILE* file) {
  BitFileWriter* result = malloc(sizeof(BitFileWriter));
  result->file = file;
  result->curByte = 0;
  result->bitsWrittenInByte = 0;

  return result;
}

void writeBits(BitFileWriter* writer, unsigned char amount, unsigned char* source) {
  for (int i = 0; i < amount; i += 1) {
    writer->curByte <<= 1;
    writer->curByte |= source[i];

    writer->bitsWrittenInByte += 1;

    if (writer->bitsWrittenInByte == 8) {
      printf("BITS "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(writer->curByte));
      fwrite(&writer->curByte, 1, 1, writer->file);
      writer->curByte = 0;
      writer->bitsWrittenInByte = 0;
    }
  }
}

void padWithZeroes(BitFileWriter* writer) {
  if (writer->bitsWrittenInByte == 0) {
    return;
  }

  writer->curByte <<= (8 - writer->bitsWrittenInByte);
  fwrite(&writer->curByte, 1, 1, writer->file);
  writer->curByte = 0;
  writer->bitsWrittenInByte = 0;
}
