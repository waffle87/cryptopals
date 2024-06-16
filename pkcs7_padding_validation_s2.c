// https://cryptopals.com/sets/2/challenges/15
#include "util/pkcs7_padding.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

/*
 * write function that takes plaintext, determines if it has valid
 * PKCS#7 padding, and strips the padding off.
 * eg. string "ICE ICE BABY\x04\x04\x04\x04" is valid
 *     string "ICE ICE BABY\x05\x05\x05\x05" is invalid
 * (0x04 == EOT)
 */

int main() {
  char *good = "ICE ICE BABY\x04\x04\x04\x04";
  char *bad1 = "ICE ICE BABY\x05\x05\x05\x05";
  char *bad2 = "ICE ICE BABY\x01\x02\x03\x04";
  assert(padding_len((uint8_t *)good, strlen(good)) == 4);
  assert(padding_len((uint8_t *)bad1, strlen(bad1)) == -1);
  assert(padding_len((uint8_t *)bad2, strlen(bad2)) == -1);
  /*
   * stripping the padding amounts to just treating the byte string
   * as a shorter buffer (len - og len - padding_len(...)). we can
   * also allocate a smaller buffer for it, copy it there, and free
   * the original one, but in general the string can be allocated
   * on the stack as well so we don't provide any generic function
   * for stripping.
   */
  return 0;
}
