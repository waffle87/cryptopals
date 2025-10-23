#include "mt19937.h"

struct mt19937_state mt19937_init(uint32_t seed) {
  struct mt19937_state obj;
  memset(&obj, 0, sizeof(struct mt19937_state));
  obj.mt[0] = seed;
  for (size_t i = 1; i < MT19937_N; i++)
    obj.mt[i] = 0x6C078965 * (obj.mt[i - 1] ^ (obj.mt[i - 1] >> 30)) + i;
  return obj;
}

void mt19937_regenerate(struct mt19937_state *obj) {
  for (size_t i = 0; i < MT19937_N; i++) {
    uint32_t y = obj->mt[i] & 0x80000000;
    y += obj->mt[(i + 1) % MT19937_N] & 0x7FFFFFFF;
    uint32_t z = obj->mt[(i + 397) % MT19937_N];
    obj->mt[i] = z ^ (y >> 1);
    if (y % 2)
      obj->mt[i] ^= 0x9908B0DF;
  }
}

uint32_t mt19937_temper(uint32_t y) {
  y ^= y >> 11;
  y ^= (y << 7) & MT19937_B;
  y ^= (y << 15) & MT19937_C;
  y ^= y >> 18;
  return y;
}

uint32_t mt19937_extract(struct mt19937_state *obj) {
  if (!obj->idx)
    mt19937_regenerate(obj);
  uint32_t y = obj->mt[obj->idx];
  obj->idx++;
  obj->idx %= MT19937_N;
  return mt19937_temper(y);
}

uint32_t unshift_right(uint32_t val, unsigned shift) {
  uint32_t z = 0;
  for (size_t i = 0; i * shift < 32; i++) {
    uint32_t mask = (0xFFFFFFFF << (32 - shift)) >> (shift * i);
    uint32_t part = val & mask;
    val ^= part >> shift;
    z |= part;
  }
  return z;
}

uint32_t unshift_left_mask(uint32_t val, unsigned shift, uint32_t mask) {
  uint32_t z = 0;
  for (size_t i = 0; i * shift < 32; i++) {
    uint32_t part_mask = (0xFFFFFFFF >> (32 - shift)) << (shift * i);
    uint32_t part = val & part_mask;
    val ^= (part << shift) & mask;
    z |= part;
  }
  return z;
}

uint32_t mt19937_untemper(uint32_t y) {
  uint32_t res = y;
  res = unshift_right(res, 18);
  res = unshift_left_mask(res, 15, MT19937_C);
  res = unshift_left_mask(res, 7, MT19937_B);
  res = unshift_right(res, 11);
  return res;
}
