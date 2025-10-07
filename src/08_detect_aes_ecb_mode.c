// Detect AES in ECB mode
#include "blocks.h"
#include "hex.h"
#include <stdio.h>

/*
 * In [data/08.txt] there are a bunch of hex-encoded ciphertexts. one of them
 * has been encrypted with ecb. detect it. remember that the problem with ecb is
 * that it is stateless and deterministic; the same 16 byte plaintext block will
 * always produce the same 16 byte ciphertext
 */

#define LINE_LEN 322

int main(void) {
  char *ascii = NULL, line[LINE_LEN], best_key;
  unsigned char **blocks = NULL;
  int ascii_len = 0, best_score = 0, curr_score = 0, best_line_num = 1;
  FILE *input = fopen("data/08.txt", "r");
  char *best_line = (char *)malloc(LINE_LEN * sizeof(char));
  for (int line_num = 1; fgets(line, LINE_LEN, input); line_num++) {
    ascii_len = hex_to_ascii(line, &ascii);
    curr_score = 0;
    int num_blocks = chunk((unsigned char *)ascii, ascii_len, 16, &blocks);
    for (int i = 0; i < num_blocks; i++)
      for (int j = i + 1; j < num_blocks; j++)
        if (!memcmp(blocks[i], blocks[j], 16))
          curr_score++;
    if (curr_score > best_score) {
      best_score = curr_score;
      strncpy(best_line, line, LINE_LEN);
      best_line_num = line_num;
    }
  }
  printf("Line: %d: %s\n", best_line_num, best_line);
  fclose(input);
  free(ascii);
  free(best_line);
  free(blocks);
  return 0;
}
