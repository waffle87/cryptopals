// Clone an MT19937 RNG from its output
#include "mt19937.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/*
 * the internal state of mt19937 consists of 624 32 bit integers. for each batch
 * of 624 outputs, mt permutes the internal state. by permuting state regularly,
 * mt19937 achieves a period of 2**19937, which is rather large. each time
 * mt19937 is tapped, an element of its internal state is subjected to a
 * tempering function that diffuses bits through the result. the tempering
 * function is invertible; you can write an "untemper" function that takes an
 * mt19937 output and transforms it back into the corresponding element of the
 * mt19937 state array. to invert the temper transform, apply the inverse of
 * each of the operations in the temper transform in reverse order. there are
 * two kinds of operations in the tmper transform each applied twice; one is an
 * xor against a right-shifted value, and the other is an xor against a
 * left-shifted dvalue and'd with a magic number. so you'll need code to invert
 * the "right" and "left" operation. once you have "untemper" working, create a
 * new mt19937 generator, tap it for 624 outputs, untemper each ofthem to
 * recreate the state of the generator, and splice that state into a new
 * instance of the mt19937 generator. the new "spliced" generator should predict
 * the values of the original. stop and think for a second... how would you
 * modify mt19937 to make this attack hard? what would happen if you subject
 * each tempered output to a cryptographic hash?
 */

int main(void) {
  uint32_t state[MT19937_N], seed = 0xDEADBEEF;
  struct mt19937_state mt = mt19937_init(seed);
  for (int i = 0; i < MT19937_N; i++) {
    state[i] = mt19937_extract(&mt);
    state[i] = mt19937_untemper(state[i]);
  }
  struct mt19937_state cloned = mt19937_init(0);
  memcpy(cloned.mt, state, sizeof(state));
  for (int i = 0; i < MT19937_N; i++) {
    uint32_t x1 = mt19937_extract(&mt);
    uint32_t x2 = mt19937_extract(&cloned);
    printf("Original: %d\nCloned: %d\n", x1, x2);
    assert(x1 == x2);
  }
  return 0;
}
