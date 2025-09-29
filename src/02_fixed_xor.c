// Fixed XOR
#include "hex.h"
#include "xor.h"
#include <assert.h>
#include <stdio.h>

/*
 * write a function that takes two equal-length buffers and produces their xor
 * combination. if your functi-n works properly, then when you feed it the
 * string: 1c0111001f010100061a024b53535009181c
 * ... after hex decoding, and when XOR'd against:
 * 686974207468652062756c6c277320657965
 * ... should produce:
 * 746865206b696420646f6e277420706c6179
 */

int main(void) {
  static char *input1 = "1c0111001f010100061a024b53535009181c";
  static char *input2 = "686974207468652062756c6c277320657965";
  static char *output = "746865206b696420646f6e277420706c6179";
  char *input1_ascii, *input2_ascii;
  int input1_len = hex_to_ascii(input1, &input1_ascii);
  int input2_len = hex_to_ascii(input2, &input2_ascii);
  unsigned char *xord = NULL;
  char *hex = NULL;
  int res = xor((unsigned char *)input1_ascii, input1_len,
                (unsigned char *)input2_ascii, input2_len, &xord);
  if (res > -1) {
    ascii_to_hex((char *)xord, &hex);
    printf("Input: %s\n", xord);
    printf("Output: %s\n", hex);
    printf("Expected: %s\n", output);
    assert(!strcmp(hex, output));
  }
  free(input1_ascii);
  free(input2_ascii);
  free(xord);
  free(hex);
  return 0;
}
