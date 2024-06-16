// https://cryptopals.com/sets/2/challenges/10
#include "util/aes.h"
#include "util/base64.h"
#include "util/io.h"

/*
 * in CBC mode each ciphertext is added to the next plaintext block prior to
 * next call to cipher core. first plaintext block is added to the "0th
 * ciphertext block" called the initialization vector (IV).
 */

int main() {
  size_t ciphertext_len;
  uint8_t *ciphertext = base64_decode(read_non_space(stdin), &ciphertext_len);
  unsigned char *key = (unsigned char *)"YELLOW SUBMARINE", iv[16] = {0};
  unsigned char *decrypted = malloc(ciphertext_len + 16); // inl + block_size
  int decrypted_len =
      cbc_decrypt(ciphertext, ciphertext_len, key, iv, decrypted);
  fwrite(decrypted, 1, decrypted_len, stdout);
  return 0;
}
