// Byte-at-a-time ECB decryption (Simple)
#include "aes.h"
#include "base64.h"
#include "random.h"
#include <assert.h>

/*
 * copy your oracle function to a new function that encrypts buffers under ecb
 * mode using a consistent but unknown key (for instance, assign a single random
 * key, once, to a global variale). now take that same function and have it
 * append to the plaintext before encrypting the following string:
 * Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg
 * YnkK. spoiler alert: do not decode this string  now. base64 decode the string
 * before appending it. do note base64 decode the string by hand, make your code
 * do it. the point is that you don't know its contents. what you have now is a
 * function that produces 'aes_128_ecb(your_string || unknown_string,
 * random_key)'. it turns out, you can decrypt 'unknown_string' with repeated
 * calls to the oracle function. here's roughly how:
 * 1. feed identical bytes of 'your_string' to the function 1 at a time. start
 * with 1 byte 'A', then 'AA', then 'AAA' and so on. discover the block size of
 * the cipher. you know it, but do this step anyway
 * 2. detect that the function is using ecb. you already know, but do this step
 * anyway as well
 * 3. knowing the block sizee, craft an input block that is exactly 1 byte short
 * (for instance, if the block size is 8 bytes, make 'AAAAAAA'). think about
 * what the oracle function is going to put in that last byte position
 * 4. make a dictionary of every possible last byte by feeding different strings
 * to the oracle. for instance, 'AAAAAAAA', 'AAAAAAAB', 'AAAAAAAC', remembering
 * the first block of each invocation
 * 5. match the output of the one byte short input to one ofthe entries in your
 * dictionary. you've now discovered the first byte of 'unknown_string'
 * 6. repeat for the next byte
 * this is the first challenge whose solution will break real crypto. lots of
 * people know that when you encrypt something in ecb mode, you can see penguins
 * through it. not so many of them can decrypt the content of those ciphertexts,
 * and now you can. if our experience is any guideline, this attack will get you
 * code execution in security tests about once a year.
 */

const unsigned char *key;
char *secret;
size_t secret_len;

struct bytes append_encrypt_ecb(char *data, size_t data_len) {
  size_t input_len = data_len + secret_len;
  char *input = (char *)malloc(input_len * sizeof(char));
  memcpy(input, data, data_len);
  memcpy(input + data_len, secret, secret_len);
  struct bytes res = aes_encrypt_ecb(input, input_len, key, 16);
  free(input);
  return res;
}

struct bytes probe_encrypt(size_t len, char *suffix, size_t suffix_len) {
  char *input = (char *)malloc(len * sizeof(char));
  memset(input, 'A', len - suffix_len);
  memcpy(input + len - suffix_len, suffix, suffix_len);
  struct bytes res = append_encrypt_ecb(input, len);
  free(input);
  return res;
}

int main(void) {
  srand(time(NULL));
  key = random_bytes(16);
  struct bytes encoded, saved = {NULL, 0};
  int block_size, i;
  char *base64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkga"
                 "GFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdX"
                 "N0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg";
  secret_len = base64_decode(base64, strlen(base64), &secret);
  struct bytes zero_encoded = append_encrypt_ecb("", 0);
  printf("Ciphertext size: %zu\n", zero_encoded.len);
  for (i = 1; i < 32; i++) {
    encoded = probe_encrypt(i, 0, 0);
    block_size = encoded.len - zero_encoded.len;
    if (block_size > 0)
      break;
  }
  size_t res_len = zero_encoded.len - i;
  printf("Block size: %d\n", block_size);
  printf("Secret size: %zu\n", res_len);
  char ones[64] = {1};
  encoded = append_encrypt_ecb(ones, 64);
  char *detected_mode = aes_oracle(encoded.data, block_size);
  printf("%s mode\n\n", detected_mode);
  char *res = (char *)malloc(res_len * sizeof(char));
  for (size_t i = 0; i < res_len; i++) {
    size_t sz = next_block(i, block_size);
    saved = probe_encrypt(sz - i - 1, 0, 0);
    for (unsigned char c = 1; c <= 128; c++) {
      res[i] = c;
      res[i + 1] = 0;
      encoded = probe_encrypt(sz, res, i + 1);
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
