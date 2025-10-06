// AES in ECB mode
#include "aes.h"
#include "base64.h"
#include "file.h"

/*
 * the base64 encoded content in [data/07.txt] has been encrypted via aes-128 in
 * ecb mode under the key "YELLOW SUBMARINE" (case sensitive, without quotes,
 * exactly 16 characters; i like "YELLOW SUBMARINE" because it's exactly 16
 * bytes long, and now you do too). decrypt it. you know the key afterall.
 * easiest way: use openssl cipher and give it aes-128 ecb as the cipher. you
 * can obviously decrypt this using the openssl command line tool, but we're
 * having you get ecb working in code for a reason. you'll need it a lot later
 * on, and not just for attacking ecb.
 */

int main(void) {
  static char *key = "YELLOW SUBMARINE";
  char *input = NULL, *decoded = NULL;
  int file_len = read_file("data/07.txt", &input);
  int decoded_len = base64_decode(input, file_len, &decoded);
  struct bytes res = aes_decrypt_ecb((unsigned char *)decoded, decoded_len,
                                     (const unsigned char *)key, strlen(key));
  if (res.data) {
    printf("%s", res.data);
    free(res.data);
  }
  free(decoded);
  return 0;
}
