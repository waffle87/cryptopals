// CTR bitflipping
#include "aes.h"
#include "misc.h"
#include "random.h"
#include <assert.h>

/*
 * there aree people in the world that believe ctr resists bit flipping attacks
 * of the kind to which cbc mode is susceptible. re-implement the cbc
 * bitflipping exercise from earlier to use ctr mode instead of cbc mode. inject
 * an "admin=true" token.
 */

unsigned char *key;

char *strip_special_chars(char *input, size_t input_len) {
  char *res = (char *)malloc(input_len * sizeof(char));
  for (size_t i = 0; i < input_len; i++)
    if (input[i] == ';' || input[i] == '=')
      res[i] = ' ';
    else
      res[i] = input[i];
  return res;
}

struct bytes strip_encrypt_ctr(char *data, size_t data_len) {
  const char *prefix = "comment1=cooking%20MCs;userdata=";
  const char *suffix = ";comment2=%20like%20a%20pound%20of%20bacon";
  size_t prefix_len = strlen(prefix);
  size_t suffix_len = strlen(suffix);
  size_t input_len = data_len + prefix_len + suffix_len;
  char *input = (char *)malloc(input_len * sizeof(char));
  memcpy(input, prefix, prefix_len);
  memcpy(input + prefix_len, data, data_len);
  memcpy(input + prefix_len + data_len, suffix, suffix_len);
  char *stripped = strip_special_chars(input, input_len);
  struct bytes res = aes_ctr((unsigned char *)stripped, input_len, key, 16, 0);
  free(input);
  free(stripped);
  return res;
}

int main(void) {
  srand(time(NULL));
  key = random_bytes(16);
  const char *data = ":admin<true:";
  struct bytes encrypted = strip_encrypt_ctr((char *)data, strlen(data));
  encrypted.data[32] ^= 0x01;
  encrypted.data[32 + 6] ^= 0x01;
  encrypted.data[32 + 11] ^= 0x01;
  struct bytes decrypted = aes_ctr(encrypted.data, encrypted.len, key, 16, 0);
  const char *target = ";admin=true;";
  assert(contains(decrypted.data, decrypted.len, target));
  printf("Success: %s in decrypted data\n", target);
  free(encrypted.data);
  free(decrypted.data);
  return 0;
}
