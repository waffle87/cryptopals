// https://cryptopals.com/sets/3/challenges/24
#include "util/mersenne_twister.h"
#include "util/random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * a trivial stream cipher can be created out of any PRNG; use it to generate
 * a sequence of 8-bit outputs and call those outputs 'keystream'. XOR each byte
 * of plaintext with each successive byte of keystream.
 */

void mt_encrypt(unsigned char *in, int in_len, uint16_t key,
                unsigned char *out) {
  unsigned char keystream[4];
  seed((uint32_t)key);
  // extract 4 bytes at a time and use it as
  // a keystream to XOR with plaintext
  for (int i = 0; i < in_len; i++) {
    if (i % 4 == 0) {
      // prepare next 4 bytes of keystream
      uint32_t k = extract();
      keystream[0] = (unsigned char)(k & 0x000000FF);
      keystream[1] = (unsigned char)((k & 0x0000FFFF) >> 8);
      keystream[2] = (unsigned char)((k & 0x00FFFFFF) >> 16);
      keystream[3] = (unsigned char)(k >> 24);
    }
    out[i] = in[i] ^ keystream[i % 4];
  }
}

void mt_decrypt(unsigned char *in, int in_len, uint16_t key,
                unsigned char *out) {
  mt_encrypt(in, in_len, key, out);
}

int main() {
  char *test = "a long test string, longer than 4 characters!";
  size_t len = strlen(test) + 1;
  unsigned char *encr = malloc(len), *decr = malloc(len);
  if (encr == NULL || decr == NULL) {
    printf("out of memory D:\n");
    exit(1);
  }
  uint16_t key = (uint16_t)arc4random();
  mt_encrypt((unsigned char *)test, len, key, encr);
  mt_decrypt(encr, len, key, decr);
  printf("after turnaround: %s\n", decr);
  free(encr);
  free(decr);
  return 0;
}
