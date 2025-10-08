#include "random.h"

unsigned char *random_bytes(size_t num_bytes) {
  unsigned char *bytes =
      (unsigned char *)malloc(num_bytes * sizeof(unsigned char));
  for (size_t i = 0; i < num_bytes; i++)
    bytes[i] = rand();
  return bytes;
}

int random_int_inclusive(int min, int max) {
  if (min >= max)
    return min;
  int range = max - min + 1, result;
  int cutoff = (RAND_MAX / range) * range;
  do
    result = rand();
  while (result >= cutoff);
  return result % range + min;
}
