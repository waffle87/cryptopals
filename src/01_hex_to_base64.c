// Convert hex to base64
#include "base64.h"
#include "hex.h"
#include <assert.h>
#include <stdio.h>

/*
 * the string:
 * 49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
 * should produce:
 * SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
 * so go ahead and make that happen. you'll need to use this code for the rest
 * of the exercises.
 *
 */

int main(void) {
  static char *input = "49276d206b696c6c696e6720796f757220627261696e206c696b652"
                       "06120706f69736f6e6f7573206d757368726f6f6d";
  static char *output =
      "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
  char *ascii = NULL, *base64 = NULL;
  size_t str_len = hex_to_ascii(input, &ascii);
  base64_encode(ascii, str_len, &base64);
  printf("Input: %s\n", input);
  printf("Output: %s\n", base64);
  printf("Expected: %s\n", output);
  assert(!strcmp(base64, output));
  free(ascii);
  free(base64);
  return 0;
}
