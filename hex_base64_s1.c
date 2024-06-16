// https://cryptopals.com/sets/1/challenges/1
#include "util/base64.h"
#include "util/hex.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Always operate on raw bytes, never on encoded strings. Only use hex and
// base64 for pretty-printing.

int main() {
  char *str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f"
              "69736f6e6f7573206d757368726f6f6d";
  size_t hex_len = strlen(str);
  uint8_t *bytes = read_hex(str, hex_len);
  char *actual = base64_encode(bytes, hex_len / 2);
  char *expect =
      "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
  if (strcmp(actual, expect) != 0) {
    printf("does not match!!!\n"
           "expected: %s\n"
           "received: %s\n",
           expect, actual);
    return 1;
  } else
    printf("passed :^)\n");
  return 0;
}
