// https://cryptopals.com/sets/1/challenges/3
#include "util/hex.h"
#include "util/score.h"
#include "util/xor.h"
#include <string.h>

int main() {
  char *str =
      "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
  char *actual;
  char *expect = "Cooking MC's like a pound of bacon";
  (void)best_score_hex(str, &actual);
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
