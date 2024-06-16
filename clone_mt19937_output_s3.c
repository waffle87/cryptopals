// https://cryptopals.com/sets/3/challenges/23
#include "util/mersenne_twister.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * the internal state of MT19937 consists of 624 32-bit integers
 * for each batch of 624 outputs, the algorithm permutes that internal
 * state. by permuting state regularly, MT19937 achieves a period of
 * 2^19937 (which is quite large!!)
 * each time MT19937 is tapped, an element of its internal state is subjected to
 * a tempering function that diffuses bits through the result, which can be
 * inverted. to invert the temper transform, apply the inverse of each operation
 * in reverse order; one XOR against right-shifted value & one XOR against
 * left-shifted value AND'd with a magic number. so tldr we need left & right
 * invert functions.
 */

#define STATE_SIZE 624
#define TEST_SIZE 10
#define U 11UL
#define S 7UL
#define B 0x9D2C5680UL
#define T 15UL
#define C 0xEFC60000UL
#define L 18UL

static uint32_t untemper(uint32_t y) {
  y ^= y >> L;
  y ^= (y << T) & C;
  // ugly bit-by-bit solution for 7-bits shift
  // i don't know how to improve it
  y ^= (((((y << 7) ^ (y << 14)) & (1UL << 31)) |
         (((y << 7) ^ (y << 14) ^ (y << 21) ^ (y << 28)) & (1UL << 28)) |
         ((y << 7) & (1UL << 27)) |
         (((y << 7) ^ (y << 14) ^ (y << 21)) & (1UL << 26)) |
         ((y << 7) & (1UL << 24)) |
         (((y << 7) ^ (y << 14) ^ (y << 21)) & (1UL << 21)) |
         (((y << 7) ^ (y << 14)) & (1UL << 19)) | ((y << 7) & (1UL << 18)) |
         (((y << 7) ^ (y << 14)) & (1UL << 14)) | ((y << 7) & (1UL << 12)) |
         ((y << 7) & (1UL << 10)) | ((y << 7) & (1UL << 9)) |
         ((y << 7) & (1UL << 7))) &
        B);
  y ^= (y >> U) ^ (y >> (2 * U));
  return y;
}

int main() {
  uint32_t outs[STATE_SIZE], state[STATE_SIZE];
  uint32_t new[TEST_SIZE], old[TEST_SIZE];
  seed(arc4random());
  for (int i = 0; i < STATE_SIZE; i++) {
    outs[i] = extract();
    state[i] = untemper(outs[i]);
  }
  // at this point we have reconstructed MT state,
  // so we can predict further values

  // random numbers from original RNG:
  for (int i = 0; i < TEST_SIZE; i++)
    old[i] = extract();
  // setup new RNG:
  set_state(state);
  // get the random number from this copied RNG:
  for (int i = 0; i < TEST_SIZE; i++)
    new[i] = extract();
  for (int i = 0; i < TEST_SIZE; i++)
    printf("old: %10" PRIu32 ", new: %10" PRIu32 "\n", old[i], new[i]);
  if (memcmp(old, new, sizeof(old)) == 0) {
    printf("passed: RNG state reconstructed :^)\n");
    exit(EXIT_SUCCESS);
  } else {
    printf("failed D:\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}
