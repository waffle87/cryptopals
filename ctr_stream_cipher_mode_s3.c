// https://cryptopals.com/sets/3/challenges/18
#include "util/aes.h"
#include "util/base64.h"

/*
 * CTR mode: instead of encrypting plaintext, CTR mode encrypts
 * a running counter, producing a 16-byte block of keystream,
 * which is XOR'd against plaintext.
 * this mode does not require padding; when there is no more
 * plaintext, XOR'ing & keystream generation just stops.
 * decryption is identical.
 */

int main() {
  char *str = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/"
              "2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
  size_t decoded_len;
  uint8_t *decoded = base64_decode(str, &decoded_len);
  unsigned char key[16] = "YELLOW SUBMARINE";
  uint64_t nonce = 0;
  unsigned char *decrypted = malloc(decoded_len);
  if (!ctr_decrypt(decoded, decoded_len, key, nonce, decrypted)) {
    fprintf(stderr, "CTR decryption error\n");
    exit(1);
  }
  if (fwrite(decrypted, 1, decoded_len, stdout) != decoded_len) {
    perror("fwrite");
    exit(1);
  }
  return 0;
}
