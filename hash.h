int circularShift(int value) {
  return value << 1 | value >> 31;
}

int prehash(char* key) {
  int result = 0;

  while (*key != '\0') {
    result = circularShift(result);
    result += *key;
    key += 1;
  }

  return result;
}

int hash(char* key, int tableCapacity) {
  return prehash(key) % tableCapacity;
}
