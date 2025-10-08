// Implement PKCS#7 padding
#include "aes.h"

/*
 * a block cipher transforms a fixed-size block (usually 8 or 16) bytes of
 * plaintext into ciphertext. but almost never want to transform a single block;
 * we encrypt irregularly sized messages. one way we account for irregularly
 * sized messages is by padding, creating a plaintext that is an even multiple
 * of the block size. the most popular padding scheme is pkcs#7. so: pad any
 * block to a specific length by appending the number of bytes of padding to the
 * end of the block. for instance, "YELLOW SUBMARINE" ...padded to 20 bytes
 * would be "YELLOW SUBMARINE\x04\x04\x04\x04"
 */

int main(void) {
  static char *input_output = "YELLOW SUBMARINE";
  struct bytes res = pkcs7(input_output, strlen(input_output), 16);
  printf("Input: %s\n", input_output);
  printf("Output: %s\n", res.data);
  printf("Expected: %s\n", input_output);
  free(res.data);
  return 0;
}
