// Implement CBC mode
#include "aes.h"
#include "base64.h"
#include "file.h"

/*
 * cbc mode is a block cipher mode that allows us to encrypt irregularly sized
 * messages, despite the fact that a block cipher natively only transforms
 * individual blocks. in cbc mode, each ciphertext block is added to the next
 * plaintext block before the next call to the cipher core. the first plaintext
 * block, which has no associated previous ciphertext block, is added to a "fake
 * 0th ciphertext block" called the initialisation vector, or iv. implement cbc
 * mode by hand by taking the ecb function you wrote earlier, making it encrypt
 * instead of decrypt (verify this by deccrypting whatever you encrypt to test),
 * and using your xor function from the previous exercise to combine them.
 * [data/10.txt] is intelligible (somewhat) when cbc decrypted against "YELLOW
 * SUBMARINE" with an iv of all ascii 0 (\x00\x00\x00 &c). do not use openssl's
 * cbc code to do cbc mode, even to verify your results.
 */

int main(void) {
  static char *key = "YELLOW SUBMARINE";
  char *input = NULL, *decoded = NULL;
  int file_len = read_file("data/10.txt", &input);
  int decoded_len = base64_decode(input, file_len, &decoded);
  unsigned char iv[16] = {0};
  struct bytes res =
      aes_decrypt_cbc((unsigned char *)decoded, decoded_len,
                      (const unsigned char *)key, strlen(key), iv);
  if (res.data) {
    printf("%s\n", res.data);
    free(res.data);
  }
  if (decoded)
    free(decoded);
  return 0;
}
