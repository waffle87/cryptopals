// https://cryptopals.com/sets/1/challenges/8
#include "util/hex.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * data/c8_s1.txt contains one ECB-encrypted hex-encoded ciphertext.
 * note, the problem with ECB is that it is stateless and deterministic;
 * the same plaintext block will always produce the same 16 byte ciphertext.
 */

int main() {
  char *line = NULL;
  size_t capacity = 0, line_num = 1;
  ssize_t len;
  while ((len = getline(&line, &capacity, stdin)) > 0) {
    line[len - 1] = '\0'; // truncate \n's
    uint8_t *buf = read_hex(line, len - 1);
    /*
     * there are 10 blocks of 16 bytes (128bits) and now
     * we have to determine which of those could be produced
     * by AES-128 ECB (electronic code book) mode.
     * we can simply look if any pair fo those blocks
     * (10 * 9 / 2 = 45) are the same.
     */
    for (size_t i = 0; i < 10; i++)
      for (size_t j = i + 1; j < 10; j++)
        if (memcmp(buf + 16 * i, buf + 16 * j, 16) == 0)
          printf("found equal blocks...\n"
                 "line: %zu\ni: %zu\nj: %zu\n",
                 line_num, i, j);
    line_num++;
    free(buf);
  }
}
