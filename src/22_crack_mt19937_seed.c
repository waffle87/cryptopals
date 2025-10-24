// Crack an MT19937 seed
#include "mt19937.h"
#include "random.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*
 * make sure your mt19937 accepts an integer seed value. test it (verify that
 * you're getting the same sequence of outputs given a seed). write a routine
 * that performs the following operation: wait a random number of seconds
 * between, 40 and 1000, seed the rng with the current unix timestamp, waits a
 * random number of seconds again, and return the first 32 bit output of the
 * rng. you get the idea. simulate the passage of time, although you're missing
 * some of the fun of the exercise if you do that. from the 32 bit rng output,
 * discover the seed
 */

#define MIN_DELAY 40
#define MAX_DELAY 1000

void delay(int min, int max) {
  int wait = random_int_inclusive(min, max);
  printf("Waiting %d seconds... (%d minutes)\n", wait, wait / 60);
  sleep(wait);
}

uint32_t output_seed(uint32_t seed) {
  struct mt19937_state mt = mt19937_init(seed);
  return mt19937_extract(&mt);
}

int main(void) {
  srand(time(NULL));
  delay(MIN_DELAY, MAX_DELAY);
  int seed = (int)time(NULL);
  uint32_t first = output_seed(seed);
  printf("Seeded PRNG...\n");
  delay(MIN_DELAY, MAX_DELAY);
  int end = (int)time(NULL);
  uint32_t curr;
  for (curr = end; output_seed(curr) != first; curr--)
    if (!(curr % 10))
      printf(".");
  printf("\nFound seed: %d\n", curr);
  return 0;
}
