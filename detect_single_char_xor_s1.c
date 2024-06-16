// https://cryptopals.com/sets/1/challenges/4
#include "util/hex.h"
#include "util/score.h"
#include "util/xor.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *line = NULL;
  size_t capacity = 0;
  char *curr_best_line, *overall_best_line;
  int best_so_far = INT_MAX;
  FILE *f = fopen("data/c4_s1.txt", "rb");
  while (getline(&line, &capacity, f) != -1) {
    *strrchr(line, '\n') = '\0';
    int curr_score = best_score_hex(line, &curr_best_line);
    if (curr_score < best_so_far) {
      best_so_far = curr_score;
      overall_best_line = strdup(curr_best_line);
    }
    free(curr_best_line);
  }
  char *actual = overall_best_line;
  char *expect = "Now that the party is jumping\n";
  if (strcmp(actual, expect) != 0) {
    printf("does not match!!!\n"
           "expected: %s\n"
           "received: %s\n",
           expect, actual);
    return 1;
  } else
    printf("passed :^)\n%s\n", expect);
  return 0;
}
