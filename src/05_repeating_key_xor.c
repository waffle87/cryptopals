// Implement repeating-key XOR
#include "hex.h"
#include "xor.h"
#include <assert.h>
#include <stdio.h>

/*
 * here is the opening stanza of an import work of the english language:
 * Burning 'em, if you ain't quick and nimble
 * I go crazy when I hear a cymbal
 * encrypt it, under the key "ICE", using repeating-key xor. in repeating
 * key-xor, you'll sequentially apply each byte of the key; the first byte of
 * plaintext will be xor'd against I, the next C, the next E, then again for the
 * 4th byte and so on. It should come out to:
 * 0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272
 * a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f
 * encrypt a bunch of stuff using your repeating-key xor function. encrypt your
 * mail. encrypt your password file. your .sig file. get a feel for it. i
 * promise, we aren't wating your time with this.
 */

int main(void) {
  static char *input = "Burning 'em, if you ain't quick and nimble\nI go crazy "
                       "when I hear a cymbal";
  static char *key = "ICE";
  static char *output =
      "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343"
      "c2a26226324272765272a282b2f20430a652e2c652a3124333a653e"
      "2b2027630c692b20283165286326302e27282f";
  char *hex = NULL;
  unsigned char *xord = NULL;
  size_t input_len = strlen(input), key_len = strlen(key);
  int res = xor((unsigned char *)input, input_len, (unsigned char *)key,
                key_len, &xord);
  if (res == -1)
    return -1;
  ascii_to_hex((char *)xord, &hex);
  printf("Input: %s\n", input);
  printf("Output: %s\n", hex);
  printf("Expected: %s\n", output);
  assert(!strcmp(hex, output));
  free(xord);
  free(hex);
  return 0;
}
