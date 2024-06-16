#include "util/aes.h"
#include "util/base64.h"
#include "util/io.h"

int main() {
  char *b64txt = read_non_space(stdin);
  if (b64txt == NULL) {
    printf("read_non_space() failed\n");
    exit(1);
  }
  size_t cyphertext_len;
  uint8_t *cyphertext = base64_decode(b64txt, &cyphertext_len);
  unsigned char *key =
      (unsigned char *)"YELLOW SUBMARINE"; // 128bit best number :^)
  unsigned char *decrypted = malloc(cyphertext_len + 16); // inl + block_size
  EVP_CIPHER_CTX *ctx = evp_init();
  int decrypted_len =
      evp_ecb_decrypt(ctx, cyphertext, cyphertext_len, key, decrypted);
  if (decrypted_len == -1)
    handle_evp_errors();
  evp_cleanup(ctx);
  decrypted[decrypted_len] = '\0'; // NULL-terminate
  printf("decrypted text is: %s\n", decrypted);
  return 0;
}
