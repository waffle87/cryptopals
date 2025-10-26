// Create the MT19937 stream cipher and break it
#include "aes.h"
#include "mt19937.h"
#include "random.h"
#include <assert.h>

/*
 * you can create a trivial stream cipher out of any prng; use it to generate a
 * sequence of 8 bit output and call those outputs a keystream. xor each byte of
 * plaintext with each successive byte of keystream. write the function that
 * does this for mt19937 using a 16-bit seed. verify that you can encrypt and
 * decrypt properly. this code should look similar to your ctr code. use your
 * function to encrypt a known plaintext (say, 14 consecutive 'a' characters)
 * prefixed by a random number of random characters. from the ciphertext,
 * recover the "key" (the 16 bit seed). use the same idea to generate a random
 * "password reset token" using mt19937 seeded from the current time. write a
 * function to check if any given password token is actually the product of an
 * mt19937 prng seeded with the current time.
 */

uint16_t rand_key;

struct bytes mt19937_encrypt_decrypt(uint32_t key, const unsigned char *data,
                                     size_t data_len) {
  unsigned char *encrypted =
      (unsigned char *)calloc(data_len + 1, sizeof(unsigned char));
  struct mt19937_state mt = mt19937_init(key);
  for (size_t i = 0; i < data_len; i++)
    encrypted[i] = data[i] ^ mt19937_extract(&mt);
  return (struct bytes){encrypted, data_len};
}

struct bytes prefix_encrypt(unsigned char *data, size_t data_len) {
  size_t prefix_len = random_int_inclusive(2, 12);
  unsigned char *prefix = random_bytes(prefix_len);
  size_t input_len = prefix_len + data_len;
  unsigned char *input =
      (unsigned char *)malloc(input_len * sizeof(unsigned char));
  memcpy(input, prefix, prefix_len);
  memcpy(input + prefix_len, data, data_len);
  rand_key = rand() % 0xFFFF;
  struct bytes res = mt19937_encrypt_decrypt(rand_key, input, input_len);
  free(prefix);
  free(input);
  return res;
}

int main(void) {
  srand(time(NULL));
  uint32_t key = 0xC0FFEE;
  static char *data = "AAAAAAAAAAAAAA";
  size_t data_len = strlen(data);
  struct bytes encrypted =
      mt19937_encrypt_decrypt(key, (unsigned char *)data, data_len);
  struct bytes decrypted =
      mt19937_encrypt_decrypt(key, encrypted.data, encrypted.len);
  assert(!memcmp(decrypted.data, (unsigned char *)data, data_len));
  encrypted = prefix_encrypt((unsigned char *)data, data_len);
  size_t plaintext_start = encrypted.len - data_len;
  uint16_t guess;
  for (guess = 0; guess < 0xFFFF; guess++) {
    struct bytes candidate =
        mt19937_encrypt_decrypt(guess, encrypted.data, encrypted.len);
    if (!memcmp(candidate.data + plaintext_start, data, data_len))
      break;
  }
  assert(guess == rand_key);
  printf("Key 0x%04" PRIX16 " recovered\n", guess);
  return 0;
}
