#pragma once

#include <stdint.h>
#include <string.h>

#define MT19937_N 624
#define MT19937_B 0x9D2C5680
#define MT19937_C 0xEFC60000

struct mt19937_state {
  uint32_t mt[MT19937_N];
  uint32_t idx;
};

struct mt19937_state mt19937_init(uint32_t seed);
void mt19937_regenerate(struct mt19937_state *obj);
uint32_t mt19937_temper(uint32_t y);
uint32_t mt19937_extract(struct mt19937_state *obj);
uint32_t unshift_right(uint32_t val, unsigned shift);
uint32_t unshift_left_mask(uint32_t val, unsigned shift, uint32_t mask);
uint32_t mt19937_untemper(uint32_t y);
