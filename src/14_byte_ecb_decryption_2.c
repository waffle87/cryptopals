// Byte-at-a-time ECB decryption (Harder)
#include "aes.h"
#include "base64.h"
#include "blocks.h"
#include "random.h"
#include <assert.h>

/*
 * take your oracle function from [src/12_byte_ecb_decryption_1.c]. now generate
 * a random count of random bytes and prepend this string to every plaintext.
 * you are now doing: 'aes_128_ecb(random_prefix || attacker_controlled ||
 * target_bytes, random_key)'. same gaol: decrypt the target bytes. stop and
 * think for a second... what's harder than challenge #12 about doing this? how
 * would you overcome that obstacle? the hint is: you're using all the tools you
 * already have; no crazy math is required. think "STIMULUS" and "RESPONSE".
 */

const unsigned char *key;
char *secret;
size_t secret_len;
size_t prefix_len;
unsigned char *prefix;

struct bytes append_encrypt_ecb2(char *data, size_t data_len) {
  size_t input_len = prefix_len + data_len + secret_len;
  char *input = (char *)malloc(input_len * sizeof(char));
  memcpy(input, prefix, prefix_len);
  memcpy(input + prefix_len, data, data_len);
  memcpy(input + prefix_len + data_len, secret, secret_len);
  struct bytes res = aes_encrypt_ecb(input, input_len, key, 16);
  free(input);
  return res;
}

struct bytes probe_encrypt2(size_t len, char *suffix, size_t suffix_len,
                            size_t padding_len) {
  char *input = (char *)malloc((len + padding_len) * sizeof(char));
  memset(input, 'A', len + padding_len - suffix_len);
  memcpy(input + len + padding_len - suffix_len, suffix, suffix_len);
  struct bytes res = append_encrypt_ecb2(input, len + padding_len);
  free(input);
  size_t new_len = res.len - 16;
  unsigned char *new = (unsigned char *)malloc(new_len * sizeof(unsigned char));
  memcpy(new, res.data + 16, new_len);
  free(res.data);
  return (struct bytes){new, new_len};
}

int main(void) {
  srand(time(NULL));
  key = random_bytes(16);
  struct bytes encoded, saved = {NULL, 0};
  int block_size, i;
  char *base64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkga"
                 "GFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdX"
                 "N0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg";
  prefix_len = random_int_inclusive(2, 12);
  prefix = random_bytes(prefix_len);
  secret_len = base64_decode(base64, strlen(base64), &secret);
  unsigned char **blocks = NULL;
  for (i = 32; i < 64; i++) {
    encoded = probe_encrypt2(i, 0, 0, 0);
    int num_blocks = chunk(encoded.data, encoded.len, 16, &blocks);
    for (int j = 0; j < num_blocks - 1; j++)
      if (!memcmp(blocks[j], blocks[j + 1], 16))
        goto exit;
  }
exit:
  free(blocks);
  size_t padding_len = i % 16;
  printf("Prefix size: %zu\n", 16 - padding_len);
  struct bytes zero_encoded = probe_encrypt2(0, 0, 0, padding_len);
  printf("Ciphertext size: %zu\n", zero_encoded.len);
  for (i = 0; i < 32; i++) {
    encoded = probe_encrypt2(i, 0, 0, padding_len);
    block_size = encoded.len - zero_encoded.len;
    if (block_size > 0)
      break;
  }
  printf("Block size: %d\n", block_size);
  size_t res_len = zero_encoded.len - i;
  printf("Secret size: %zu\n", res_len);
  char ones[64] = {1};
  encoded = append_encrypt_ecb2(ones, 64);
  char *detected_mode = aes_oracle(encoded.data, block_size);
  printf("%s mode\n\n", detected_mode);
  char *res = (char *)malloc(res_len * sizeof(char));
  for (size_t i = 0; i < res_len; i++) {
    size_t sz = next_block(i, block_size);
    saved = probe_encrypt2(sz - i - 1, 0, 0, padding_len);
    for (unsigned char ch = 1; ch <= 128; ch++) {
      res[i] = ch;
      res[i + 1] = 0;
      encoded = probe_encrypt2(sz, res, i + 1, padding_len);
      if (!memcmp(encoded.data, saved.data, sz))
        break;
    }
  }
  printf("%s\n", res);
  assert(!strcmp(res, secret));
  free(res);
  free(zero_encoded.data);
  free(encoded.data);
  free(saved.data);
  return 0;
}
