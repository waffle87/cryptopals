#include <stdio.h>
#include <stdint.h>

#define ALPHABET_SZ 26
#define SIGNIFICANT_LETTERS 20
#define FREQ_ORDER " etaoinshrdlcumwfgypbvkjxqz"
//^ see https://en.wikipedia.org/wiki/Letter_frequency

struct freq {
  char c;
  int freq;
};

static int cmp_by_freq(const void *x, const void *y);
static int char_canonical_order(char c);
static int score_step(uint8_t *str, size_t len, int n, size_t start, size_t step);
static int score(uint8_t *str, size_t len, int n);

/*
 * returns a best score for the encoded string in hex and inits
 * res_str, which points to the best string.
 * it is malloc'd (so free() it!!!)
 * "score" is a measurement of how likely the input string is
 * to be human readable. lower score == better
 */
int best_score_hex(char *hex_str, char **res_str);

/*
 * returns a best score for byte buffer and inits res_str,
 * which points to the best string.
 * it is malloc'd (so free() it!!!)
 */
int best_score_raw(uint8_t *bytes, size_t bytes_len, char **res_str);

/*
 * returns the key corresponding to the best score for byte buffer when
 * only every step'th byte starting from byte 'start' is taken into accont
 */
uint8_t best_key_step(uint8_t *bytes, size_t bytes_len, size_t start, size_t step);
