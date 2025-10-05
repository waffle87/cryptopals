// Detect single-character XOR
#include "hex.h"
#include "stats.h"
#include "xor.h"
#include <stdio.h>

/*
 * one of the 60-character strings in [data/04.txt] has been encrypted by
 * singe-character xor. find it. your code from #3 should help
 */

#define LINE_LEN 61

int main(void) {
  char *ascii = NULL, line[LINE_LEN], best_key;
  unsigned char *xord = NULL;
  int s1_len = 0, best_score = 0, curr_score = 0, best_line_num = 1;
  FILE *input = fopen("data/04.txt", "r");
  char *best_line = (char *)malloc(LINE_LEN * sizeof(char));
  for (int line_num = 1; fgets(line, LINE_LEN, input); line_num++) {
    s1_len = hex_to_ascii(line, &ascii);
    best_key =
        find_single_byte_xor((unsigned char *)ascii, s1_len, frequency_score);
    unsigned char key_table[] = {best_key};
    xor((unsigned char *)ascii, s1_len, key_table, sizeof(char), &xord);
    curr_score = frequency_score(xord, s1_len);
    if (curr_score > best_score) {
      best_score = curr_score;
      strncpy(best_line, (char *)xord, LINE_LEN);
      best_line_num = line_num;
    }
  }
  printf("Line: %d: %s\n", best_line_num, best_line);
  fclose(input);
  free(ascii);
  free(best_line);
  free(xord);
  return 0;
}
