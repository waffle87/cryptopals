// https://cryptopals.com/sets/3/challenges/21
#include "util/mersenne_twister.h"
#include <stdio.h>

int main() {
  seed(0);
  // extra basic statistical test; checks whether
  // lowest significant bit is random enough :^)
  //(borrowed from testU01)
  int even = 0, odd = 0;
  for (int i = 0; i < 10000; i++) {
    uint32_t r = extract();
    if (r % 2 == 0)
      even++;
    else
      odd++;
  }
  printf("even: %d, odd: %d\n", even, odd);
  return 0;
}
