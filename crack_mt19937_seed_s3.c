// https://cryptopals.com/sets/3/challenges/22
#include "util/mersenne_twister.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*
 * wait a random number of seconds between 40-1000
 * seeds RNG with current unix timestamp
 * wait a random number of seconds again
 * returns first 32-bit output of RNG
 * profit
 */

#define LO 40
#define HI 1000

static uint32_t real_seed;

static uint32_t simulation() {
  uint32_t wait_time = arc4random_uniform(HI - LO + 1) + LO;
  sleep((int)wait_time);
  real_seed = (uint32_t)time(NULL);
  seed(real_seed);
  wait_time = arc4random_uniform(HI - LO + 1) + LO;
  sleep((int)wait_time);
  return extract();
}

static uint32_t detect_seed(uint32_t first_val) {
  uint32_t now = (uint32_t)time(NULL);
  for (uint32_t s = now; s > now - 3 * HI; s--) {
    seed(s);
    if (extract() == first_val)
      return s;
  }
  printf("could not find seed D:\n");
  return 0;
}

int main() {
  uint32_t rand_val = simulation();
  uint32_t detected_seed = detect_seed(rand_val);
  printf("real_seed = %" PRIu32 ", detected seed = %" PRIu32 "\n", real_seed,
         detected_seed);
  return 0;
}
