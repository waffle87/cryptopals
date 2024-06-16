// https://cryptopals.com/sets/2/challenges/11
#include "util/aes.h"
#include "util/random.h"
#include <openssl/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * tldr: generate random AES key (16 bytes)
 * create function that encrypts data under an unknown
 * (to us) key. eg. encryption_oracle(input)
 *                  => [foo bar baz]
 * function should append 5-10 bytes (count is random)
 * before and after the plaintext.
 * function should encrypt under ECB half the time,
 * and CBC the other half (use random IV's)
 * detect block cipher mode the function is using.
 */

#define BLOCK_SIZE 16
enum mode { CBC, ECB };

static int oracle(unsigned char *in, size_t in_len, unsigned char *out,
                  enum mode *used_mode) {
  unsigned char key[BLOCK_SIZE];
  int encrypted_len;
  init_with_random_bytes(key, BLOCK_SIZE);
  // append 5-10 bytes at beginning & end
  uint32_t begin_bytes = arc4random_uniform(6) + 5;
  uint32_t end_bytes = arc4random_uniform(6) + 5;
  // allocate new buffer for input and extra bytes
  size_t new_in_len = in_len + begin_bytes + end_bytes;
  unsigned char *new_in = malloc(new_in_len);
  if (new_in == NULL)
    return -1;
  // initialize extra bytes randomly
  arc4random_buf(new_in, begin_bytes);
  memcpy(new_in + begin_bytes, in, in_len);
  arc4random_buf(new_in + begin_bytes + in_len, end_bytes);
  // randomly encrypt with ECB or CBC (uniformly)
  if (arc4random_uniform(2) == 0) {
    unsigned char iv[BLOCK_SIZE];
    init_with_random_bytes(iv, BLOCK_SIZE);
    encrypted_len = cbc_encrypt(new_in, new_in_len, key, iv, out);
    *used_mode = CBC;
  } else {
    EVP_CIPHER_CTX *ctx = evp_init();
    encrypted_len = evp_ecb_encrypt(ctx, new_in, new_in_len, key, out);
    evp_cleanup(ctx);
    *used_mode = ECB;
  }
  free(new_in);
  return encrypted_len;
}

/*
 * send 43 identical bytes (0x00 in this case), that is enough to handle
 * random addition of 5-10 bytes at beginning & end, and check if the second
 * and third blocks returned are identical. they should be if it's ECB,
 * and they are most likely not if it's CBC.
 */

void guess() {
  unsigned char zeros[43] = {0};
  unsigned char *encrypted = malloc(43 + BLOCK_SIZE);
  enum mode used_mode, guessed_mode;
  (void)oracle(zeros, 43, encrypted, &used_mode);
  if (memcmp(encrypted + BLOCK_SIZE, encrypted + 2 * BLOCK_SIZE, BLOCK_SIZE) ==
      0)
    guessed_mode = ECB;
  else
    guessed_mode = CBC;
  printf("used mode:    %s\n", used_mode == ECB ? "ECB" : "CBC");
  printf("guessed_mode: %s\n", guessed_mode == ECB ? "ECB" : "CBC");
  if (used_mode == guessed_mode)
    printf("gussed correct :^)\n");
  else
    printf("wahh guess incorrect :^(\n");
  free(encrypted);
}

int main() {
  for (int i = 0; i < 10; i++)
    guess();
  return 0;
}
