#include <assert.h>
#include <string.h>
#include "hamming.h"

/*
 * precalculated number 1-bits in corresponding bytes from 0 to 255 or
 * (00000000 -> 0) to (11111111 -> 8). formatted as a readable 16x16 table.
 * see htpps://en.wikipedia.org/wiki/Hamming_weight for more.
 * precalculated table version is not most efficient approach,
 * but simplest by use of 'precalculate_pop_count'.
 */
static size_t table[] = {
//0 1 2 3 4 5 6 7 8 9 A B C D E F
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4, //0x0
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5, //0x1
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5, //0x2
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x3
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5, //0x4
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x5
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x6
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0x7
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5, //0x8
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x9
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0xA
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0xB
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0xC
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0xD
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0xE
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8  //0xF
};

size_t pop_count(uint8_t byte) {
  return table[byte];
}

size_t hamming_distance(char *s1, char *s2) {
  size_t len1 = strlen(s1), len2 = strlen(s2), distance = 0;
  assert(len1 == len2);
  for (unsigned long i = 0; i < len1; i++)
    distance += pop_count(s1[i] ^ s2[i]);
  return distance;
}

size_t hamming_distance_bytes(uint8_t *b1, uint8_t *b2, size_t len) {
  size_t distance = 0;
  for (unsigned long i = 0; i < len; i++)
    distance += pop_count(b1[i] ^ b2[i]);
  return distance;
}
