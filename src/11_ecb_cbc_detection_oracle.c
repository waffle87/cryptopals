// An ECB/CBC detection oracle
#include "aes.h"
#include "random.h"
#include <assert.h>

/*
 * now that you have ecb and cbc working: write a function to generate a random
 * aes key; that's just 16 random bytes. write a function that encrypts that
 * data under an unknown key -- that is, a function that generates a random key
 * and encrypts under it. the function looks like: encryption_oracle(your_input)
 * => [meaningless jibber jabber]
 * under the hood, have the function append 5-10 bytes (count chosen randomly)
 * before the plaintext and 5-10 bytes after the plaintext. now, have the
 * function choose to encrypt under ecb 1/2 of the time, and under cbc the other
 * half (just use random iv's each time for cbc). use rand(2) to decide which to
 * use. detect the block cipher mode the function is using each time. you should
 * end up with a piece of code that, pointed at a block box that might be
 * encrypting ecb or cbc, tells you which is happening.
 */

struct bytes random_prefix_suffix(char *data, size_t data_len) {
  size_t prefix_len = random_int_inclusive(5, 10);
  size_t suffix_len = random_int_inclusive(5, 10);
  unsigned char *prefix = random_bytes(prefix_len);
  unsigned char *suffix = random_bytes(suffix_len);
  size_t total_len = prefix_len + data_len + suffix_len;
  unsigned char *str =
      (unsigned char *)malloc(total_len * sizeof(unsigned char));
  memcpy(str, prefix, prefix_len);
  memcpy(str + prefix_len, data, data_len);
  memcpy(str + prefix_len + data_len, suffix, suffix_len);
  free(prefix);
  free(suffix);
  return (struct bytes){str, total_len};
}

int main(void) {
  srand(time(NULL));
  struct bytes data, encoded;
  char *mode, ones[64] = {1};
  unsigned char iv[16] = {0};
  unsigned char *key;
  for (int i = 1; i <= 100; i++) {
    data = random_prefix_suffix(ones, 64);
    key = random_bytes(16);
    if (rand() % 2) {
      mode = "ECB";
      encoded = aes_encrypt_ecb((char *)data.data, data.len, key, 16);
    } else {
      mode = "CBC";
      encoded = aes_encrypt_cbc((char *)data.data, data.len, key, 16, iv);
    }
    char *detected_mode = aes_oracle(encoded.data, 16);
    printf("round %d:\n", i);
    printf("mode: %s\n", mode);
    printf("detected: %s\n\n", detected_mode);
    assert(!strcmp(mode, detected_mode));
    free(data.data);
    free(encoded.data);
    free(key);
  }
  return 0;
}
