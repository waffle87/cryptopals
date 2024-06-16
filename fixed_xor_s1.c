// https://cryptopals.com/sets/1/challenges/2
#include "util/xor.h"
#include <string.h>

int main() {
  char *str = "1c0111001f010100061a024b53535009181c";
  char *cmp = "686974207468652062756c6c277320657965";
  char *actual = xor_hex(str, cmp);
  char *expect = "746865206b696420646f6e277420706c6179";
  if (strcmp(actual, expect) != 0) {
    printf("does not match!!!\n"
           "expected: %s\n"
           "received: %s\n",
           expect, actual);
    return 1;
  } else
    printf("passed :^)\n");
  return 0;
}
