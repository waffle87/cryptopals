// Implement the MT19937 Mersenne Twister RNG
#include "mt19937.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

/*
 * you can get the pseudocode for this from wikipedia. if you're writing in
 * python, ruby, or (gah) php, your language is probably already giving you
 * mt19937 as 'rand()'. don't use it. write the rng yourself.
 */

int main(void) {
  static uint32_t seed = 5489;
  static uint32_t known_vals[] = {
      -795755684, 581869302,  -404620562, -708632711,  545404204,
      -133711905, -372047867, 949333985,  -1579004998, 1323567403};
  struct mt19937_state mt = mt19937_init(seed);
  for (int i = 0; i < 10; i++) {
    uint32_t extracted = mt19937_extract(&mt);
    printf("%" PRIu32 "\n", extracted);
    assert(extracted == known_vals[i]);
  }
  return 0;
}
