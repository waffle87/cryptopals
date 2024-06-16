#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "score.h"
#include "hex.h"
#include "xor.h"

static int cmp_by_freq(const void *x, const void *y) {
  const struct freq *sx = x;
  const struct freq *sy = y;
  return sy->freq - sx->freq; //descending order
}

static int char_canonical_order(char c) {
  assert((c >= 'a' && c <= 'z') || c == ' ');
  char canonical_order[] = FREQ_ORDER;
  char *cp = strchr(canonical_order, c);
  return cp - canonical_order;
}

/*
 * this scoring function is based on frequency of english letters.
 * we count occurences of all letters and space and sort them
 * based on frequencies, producing a permutation of 26 letters (and Space character).
 * then add up distances between positions of top N letters (20, for example (SIGNIFICANT_LETTERS constant)) of our orders
 * with the canonical order.
 */
static int score_step(uint8_t *str, size_t len, int n, size_t start, size_t step) {
  assert(str != NULL);
  assert(n >= 1 && n <= ALPHABET_SZ + 1); //+1 for space character
  struct freq f[ALPHABET_SZ + 1];
  for (int i = 0; i < ALPHABET_SZ + 1; i++) {
    struct freq j;
    if (i == ALPHABET_SZ)
      j.c = ' ';
    else
      j.c = 'a' + i;
    j.freq = 0;
    f[i] = j;
  }
  //count frequencies
  int score = 0;
  for (size_t i = start; i < len; i += step) {
    char c = str[i];
    if (c >= 'a' && c <= 'z')
      f[c - 'a'].freq++;
    else if (c >= 'A' && c <= 'Z')
      f[c - 'A'].freq++;
    else if (c == ' ')
      f[ALPHABET_SZ].freq++;
    else if (!ispunct(c))
      score += n; //"penalty" for non-punctuation characters
  }
  //sort & calculate final score!
  qsort(f, ALPHABET_SZ + 1, sizeof(struct freq), cmp_by_freq);
  for (int i = 0; i < n; i++)
    score += abs(char_canonical_order(f[i].c) - i);
  return score;
}

static int score(uint8_t *str, size_t len, int n) {
  return score_step(str, len, n, 0, 1);
}

int best_score_hex(char *hex_str, char **res_str) {
  size_t hex_len = strlen(hex_str);
  size_t bytes_len = hex_len / 2;
  uint8_t *bytes = read_hex(hex_str, hex_len);
  return best_score_raw(bytes, bytes_len, res_str);
}

int best_score_raw(uint8_t *bytes, size_t bytes_len, char **res_str) {
  int min_score = INT_MAX;
  uint8_t *best_string;
  uint8_t cypher, key = 0;
  for (;;) {
    //xor them with key
    uint8_t *xor_d = xor_with_byte_key(bytes, key, bytes_len);
    //score string & check for improvement
    int curr_score = score(xor_d, bytes_len, SIGNIFICANT_LETTERS);
    if (curr_score < min_score) {
      min_score = curr_score;
      cypher = key;
      best_string = xor_d;
    } else
      free(xor_d);
    key++;
    if (key == 0)
      break; //pass complete
  }
  char *best_str_terminated = malloc(bytes_len + 1);
  memcpy(best_str_terminated, best_string, bytes_len);
  best_str_terminated[bytes_len] = '\0';
  free(best_string);
  *res_str = best_str_terminated;
  return min_score;
}

uint8_t best_key_step(uint8_t *bytes, size_t bytes_len, size_t start, size_t step) {
  int min_score = INT_MAX;
  uint8_t best_key = 0, key = 0;
  for (;;) {
    uint8_t *xor_d = xor_with_byte_key(bytes, key, bytes_len);
    int curr_score = score_step(xor_d, bytes_len, SIGNIFICANT_LETTERS, start, step);
    if (curr_score < min_score) {
      min_score = curr_score;
      best_key = key;
    }
    free(xor_d);
    if (++key == 0)
      break; //pass complete
  }
  return best_key;
}
