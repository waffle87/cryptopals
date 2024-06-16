// https://cryptopals.com/sets/1/challenges/5
#include "util/hex.h"
#include "util/xor.h"
#include <stdint.h>
#include <string.h>

int main() {
  uint8_t msg[] = "Burning 'em, if you ain't quick and nimble\n"
                  "I go crazy when I hear a cymbal",
          key[] = "ICE";
  int msg_len = sizeof msg - 1,
      key_len = sizeof key_len - 1; //-1 because \0 at end
  uint8_t *xor_d = xor_with_repeating_key(msg, msg_len, key, key_len);
  char *actual = write_hex(xor_d, msg_len);
  char *expect = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a262"
                 "26324272765272"
                 "a282b2f20430a652e2c652a3124333a653e2b2027630c692b202831652863"
                 "26302e27282f";
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
