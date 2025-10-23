// Break fixed-nonce CTR statistically
#include "aes.h"
#include "base64.h"
#include "random.h"
#include "stats.h"

/*
 * take your ctr encrypt/decrypt function and fix its nonce value to 0. generate
 * a random aes key. in successive encryption (not in one big running ctr),
 * encrypt each line of the base64 decodes of [data/20.txt], producing multiple
 * independent ciphertexts. because the ctr nonce wasn't randomised for each
 * encryption, each ciphertext has been encrypted against the same keystream.
 * this is very bad. understanding that, like most stream ciphers (incuding rc4,
 * and obviously any block cipher run in ctr mode), the actual "encryption" of a
 * byte of data boils down to a single xor operation, it should be plain that
 * 'ciphertext_byte ^ plaintext_byte = keystream_byte'. and since the keystream
 * is the same for every ciphertext: 'ciphertext_byte ^ keystream_byte =
 * plaintext_byte'. as challenge 19 seems somewhat redundant, i have chose to
 * simply do 20 instead
 */

#define LINE_LEN 172
#define NUM_LINE 60

int main(void) {
  srand(time(NULL));
  char line[LINE_LEN], *decoded = NULL;
  struct bytes ciphers[NUM_LINE];
  int longest = 0;
  const unsigned char *rand_key = random_bytes(16);
  const uint64_t nonce = 0;
  FILE *input = fopen("data/20.txt", "r");
  for (int line_num = 1; fgets(line, LINE_LEN, input); line_num++) {
    int decoded_len = base64_decode(line, strlen(line), &decoded);
    if (decoded_len > longest)
      longest = decoded_len;
    ciphers[line_num - 1] =
        aes_ctr((unsigned char *)decoded, decoded_len, rand_key, 16, nonce);
  }
  int **ascii_chars = (int **)malloc(longest * sizeof(int *));
  for (int i = 0; i < longest; i++)
    ascii_chars[i] = (int *)malloc(256 * sizeof(int));
  for (int i = 0; i < longest; i++) {
    for (int j = 0; j < 256; j++) {
      for (int k = 0; k < NUM_LINE; k++) {
        if (ciphers[k].len >= (unsigned int)i) {
          unsigned char res = j ^ ciphers[k].data[i];
          ascii_chars[i][j] += frequency_score(&res, 1);
        }
      }
    }
  }
  unsigned char *key = (unsigned char *)malloc(longest * sizeof(unsigned char));
  for (int i = 0; i < longest; i++) {
    unsigned char best = 0;
    int best_score = 0;
    for (int j = 0; j < 256; j++) {
      if (ascii_chars[i][j] > best_score) {
        best = j;
        best_score = ascii_chars[i][j];
      }
    }
    key[i] = best;
  }
  for (size_t i = 0; i < NUM_LINE; i++) {
    for (size_t j = 0; j < ciphers[i].len; j++)
      printf("%c", ciphers[i].data[j] ^ key[j]);
    printf("\n");
  }
  free(key);
  fclose(input);
  free(decoded);
  free(ascii_chars);
  return 0;
}
