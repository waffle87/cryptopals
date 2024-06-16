/*
 * base on pseudocode from
 * https://en.wikipedia.org/wiki/Mersenne_Twister
 * MT19937 version (32-bit word)
 */

#include "mersenne_twister.h"

//MT-32 algorithm coefficients
#define W 32UL
#define N 624UL
#define M 397UL
#define R 31UL
#define A 0x9908B0DFUL
#define U 11UL
#define D 0xFFFFFFFFUL
#define S 7UL
#define B 0x9D2C5680UL
#define T 15UL
#define C 0xEFC60000UL
#define L 18UL
#define F 1812433253UL //initialization constant

//derived constants
#define LOWER_MASK ((1UL << R) - 1)
#define UPPER_MASK (~LOWER_MASK)

//global state
static uint32_t MT[N], idx = N + 1; //essentially unseeded

//generate next 'N' values
static void twist() {
  for (uint32_t i = 0; i < N; i++) {
    uint32_t x = (MT[i] & UPPER_MASK) + (MT[(i + 1) % N] & LOWER_MASK);
    uint32_t x_a = x >> 1;
    if ((x % 2) != 0)
      x_a ^= A;
    MT[i] = MT[(i + M) % N] ^ x_a;
  }
  idx = 0;
}

void set_state(uint32_t *new_state) {
  idx = N;
  for (uint32_t i = 0; i < N; i++)
    MT[i] = *new_state++;
}

void seed(uint32_t seed) {
  idx = N;
  MT[0] = seed;
  for (uint32_t i = 1; i < N; i++)
    MT[i] = F * (MT[i - 1] ^ (MT[i - 1] >> (W - 2))) + i;
}

uint32_t extract() {
  if (idx >= N) {
    if (idx > N)
      seed(5489); //default seed
    twist();
  }
  uint32_t y = MT[idx++];
  y ^= (y >> U) & D;
  y ^= (y << S) & B;
  y ^= (y << T) & C;
  y ^= (y >> L);
  return y;
}
