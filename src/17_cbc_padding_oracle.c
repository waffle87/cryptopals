// The CBC padding oracle
#include "aes.h"
#include "base64.h"
#include "blocks.h"
#include "random.h"
#include <assert.h>

/*
 * this is the best-knowwn attach on modern block-cipher cryptography. combine
 * your padding code and your cbc code to write two functions. the first
 * function should select at random one of the 10 strings in [data/17.txt]
 * ...generate a random aes key (which it should save for all future
 * encryption), pad the string out to the 16-byte aes block size, and cbc
 * encrypt it under that key, providing the ciphertext and the iv. the second
 * function should consume the ciphertext produced by the first function,
 * decrypt it, check its padding, and return true or false depending on whether
 * the padding is valid. what you're doing here... this pair of functions
 * approximates aes-cbc encryption as its deployed serverside in web
 * applications; the second function models the server's consumption of an
 * ecrypted session token, as if it was a cookie. it turns out that it's
 * possible to decrypt the ciphertexts provided by the first function. the
 * decryption here depends on a side-channel leak by the decryption function.
 * the leak is the error message that the padding is valid or not. you can find
 * 100 web pages on how this attack works, but the fundamental insight behind
 * this attack is that the byte 01h is valid padding, and occur 1/256 trials of
 * "randomised" plaintexts produced by decrypting a tampered ciphertext. 02h in
 * isolation is not valid padding. 02h 02h is valid padding, but is much less
 * likely to occur randomly than 01h. so you can assume that if you corrupt a
 * decryption and it had valid padding, you know what that padding byte is. it
 * is easy to get tripped up on the fact that cbc plaintexts are "padded".
 * padding oracles have nothing to with the actual padding on a cbc plaintext.
 * it's an attack that targets a specific bit of code that handles decryption.
 * you can mount a padding oracle on any cbc block, whether it's padded or not.
 */

#define LINE_LEN 81

unsigned char *key;
size_t key_len;
unsigned char iv[16] = {0};

bool is_valid_padding(unsigned char *encrypted, size_t encrypted_len) {
  struct bytes decrypted =
      aes_decrypt_cbc((unsigned char *)encrypted, encrypted_len,
                      (const unsigned char *)key, key_len, iv);
  if (!decrypted.data)
    return false;
  free(decrypted.data);
  return true;
}

struct bytes padding_attack(unsigned char *encrypted, size_t encrypted_len,
                            unsigned char *iv) {
  unsigned char **blocks = NULL, *prev_block = iv, attack_block[32] = {0};
  char *plaintext = (char *)malloc(1 + (encrypted_len * sizeof(char)));
  plaintext[encrypted_len] = 0;
  size_t num_blocks = chunk(encrypted, encrypted_len, 16, &blocks);
  for (size_t block = 0; block < num_blocks; block++) {
    memcpy(attack_block + 16, blocks[block], 16);
    for (size_t i = 0; i < 16; i++) {
      char pad_byte = i + 1;
      for (size_t j = 0; j < i; j++)
        attack_block[15 - j] =
            prev_block[15 - j] ^ plaintext[(16 * block) + 15 - j] ^ pad_byte;
      for (int byte = 0; byte < 256; byte++) {
        attack_block[15 - i] = byte;
        if (is_valid_padding(attack_block, sizeof(attack_block))) {
          char decrypted = prev_block[15 - i] ^ byte ^ pad_byte;
          int idx = (16 * block) + 15 - i;
          plaintext[idx] = decrypted;
          break;
        }
      }
    }
    memcpy(prev_block, blocks[block], 16);
  }
  struct bytes res = pkcs7_strip((unsigned char *)plaintext, encrypted_len);
  return res;
}

int main(void) {
  char *decoded = NULL, line[LINE_LEN];
  struct bytes encrypted, decrypted;
  key = random_bytes(16);
  key_len = 16;
  FILE *input = fopen("data/17.txt", "r");
  for (int line_num = 1; fgets(line, LINE_LEN, input); line_num++) {
    int decoded_len = base64_decode(line, strlen(line), &decoded);
    encrypted = aes_encrypt_cbc(decoded, decoded_len,
                                (const unsigned char *)key, key_len, iv);
    if (encrypted.data) {
      decrypted = padding_attack(encrypted.data, encrypted.len, iv);
      printf("%s\n", decrypted.data);
      assert(!strncmp((char *)decrypted.data, decoded, decoded_len));
      free(encrypted.data);
      free(decrypted.data);
    }
  }
  free(decoded);
  return 0;
}
