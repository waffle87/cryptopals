// Single-byte XOR cipher
#include "hex.h"
#include "stats.h"
#include "xor.h"
#include <assert.h>
#include <stdio.h>

/*
 * the hex encoded string:
 * 1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736
 * ... has been xor'd against  single character. find the key, decrypt the
 * message. you can do this by hand, but don't: wrrite code to do it for you.
 * how? devise some method for "scoring" a piece of english plaintext. character
 * frequency is a good metric. evaluate each output and chose the one with the
 * best score.
 */

int main(void) {
  static char *input =
      "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
  static char *output = "Cooking MC's like a pound of bacon";
  char *ascii;
  unsigned char *xord = NULL;
  size_t input_len = hex_to_ascii(input, &ascii);
  unsigned char res =
      find_single_byte_xor((unsigned char *)ascii, input_len, &frequency_score);
  if (res > 0) {
    unsigned char table[] = {res};
    xor((unsigned char *)ascii, input_len, table, sizeof(char), &xord);
    printf("Input: %s\n", input);
    printf("Key: %c\n", res);
    printf("Output: %s\n", xord);
    assert(!strcmp((char *)xord, output));
  }
  free(ascii);
  free(xord);
  return 0;
}
