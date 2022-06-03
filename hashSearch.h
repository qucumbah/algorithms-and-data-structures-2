int circularShift(int value) {
  return value << 1 | value >> 31;
}

int hash(char* key, int length) {
  int result = 0;

  for (int i = 0; i < length; i += 1) {
    result = circularShift(result);
    result += key[i];
  }

  return result;
}
