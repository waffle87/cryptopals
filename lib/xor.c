#include "xor.h"

int xor(const unsigned char *s1, const size_t s1_len, const unsigned char *s2,
        const size_t s2_len, unsigned char **res) {
  *res = (unsigned char *)malloc((s1_len + 1) * sizeof(char));
  size_t i;
  for (i = 0; i < s1_len; i++)
    (*res)[i] = s1[i] ^ s2[i % s2_len];
  (*res)[i] = 0;
  return 0;
}

char find_single_byte_xor(const unsigned char *input, const size_t input_len,
                          int (*score_function)(const unsigned char *,
                                                const size_t)) {
  unsigned char *buf;
  int curr_best_key = 0, best_score = 0, curr_score;
  for (unsigned char i = 0; i < UINT8_MAX; i++) {
    unsigned char key_table[] = {i};
    xor(input, input_len, key_table, sizeof(char), &buf);
    curr_score = score_function(buf, input_len);
    if (curr_score > best_score) {
      curr_best_key = i;
      best_score = curr_score;
    }
  }
  return curr_best_key;
}

int hamming_distance(const unsigned char *s1, const size_t s1_len,
                     const unsigned char *s2, const size_t s2_len) {
  if (s1_len != s2_len)
    return -1;
  int score = 0, curr_xor = 0;
  for (size_t i = 0; i < s1_len; i++) {
    curr_xor = s1[i] ^ s2[i];
    do
      score += curr_xor & 1;
    while (curr_xor >>= 1);
  }
  return score;
}

int find_xor_key_len(const unsigned char *input, const size_t input_len,
                     const size_t min_key_len, const size_t max_key_len) {
  size_t dist_len = 0, best_key_len = 0, max_dist, best_dist = INT_MAX;
  float dist = 0.0;
  for (size_t key_len = min_key_len; key_len <= max_key_len; key_len++) {
    dist = 0.0;
    max_dist = (input_len / key_len) - 1;
    for (dist_len = 0; dist_len < max_dist; dist_len++) {
      int hamming =
          hamming_distance(input + (key_len * dist_len), key_len,
                           input + (key_len * (dist_len + 1)), key_len);
      dist += (float)hamming / key_len;
    }
    dist /= (float)max_dist;
    if (dist < best_dist) {
      best_dist = dist;
      best_key_len = key_len;
    }
  }
  return best_key_len;
}
