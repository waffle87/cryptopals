// https://cryptopals.com/sets/2/challenges/9
#include "util/pkcs7_padding.h"
#include <stdio.h>
#include <string.h>

/*
 * a block cipher transforms fixed-size block (8/16 bytes) of plaintext into
 * ciphertext. but we rarely want to transform a single block; we encrypt
 * irregularly-sized messages. the odd size of these messages can be accounted
 * for using padding, specifically PKCS#7 scheme. eg. "YELLOW SUBMARINE" padded
 * to 20 bytes would be "YELLOW SUBMARINE\x04\x04\x04\x04" (0x04 indicates EOT
 * or end-of-transmission)
 */

int main() {
  uint8_t *str = (uint8_t *)"YELLOW SUBMARINE", *res;
  size_t curr_len = strlen((char *)str);
  ssize_t new_len = add_padding(str, curr_len, 20, &res);
  if (new_len == -1) {
    fprintf(stderr, "add_padding() failed\n");
    exit(1);
  }
  fwrite(res, 1, new_len, stdout);
  free(res);
  printf("curr len: %zu\nnew_len: %zd\n", curr_len, new_len);
}
