#include "util/base64.h"
#include "util/hamming.h"
#include "util/io.h"
#include "util/score.h"
#include "util/xor.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEBUG

static double average(double *xs, size_t len) {
  size_t count = 0;
  double total = 0;
  for (size_t i = 0; i < len; i++) {
    count++;
    total += xs[i];
  }
  return total / count;
}

static void print_key(uint8_t *key, size_t len) {
  printf("key=\"");
  for (size_t i = 0; i < len; i++)
    printf("%c", key[i]);
  printf("\"\n");
}

int main() {
  FILE *stream = stdin;
  char *b64text = read_non_space(stream);
  if (b64text == NULL) {
    printf("read_non_space() failed\n");
    exit(1);
  }
  size_t byte_len;
  uint8_t *cyphertext = base64_decode(b64text, &byte_len);
  if (cyphertext == NULL) {
    printf("base64_decode() failed\n");
    exit(1);
  }
  size_t key_size, best_size = 0;
  double best_dist = 1e9;
  int n = 15;
  for (key_size = 2; key_size <= 40; key_size++) {
    double dist[n];
    uint8_t *curr = cyphertext;
    for (int i = 0; i < n; i++) {
      dist[i] =
          ((double)hamming_distance_bytes(curr, curr + key_size, key_size)) /
          key_size;
      curr += 2 * key_size;
    }
    double norm_dist = average(dist, n);
#ifdef DEBUG
    printf("key size: %zu, norm dist: %f\n", key_size, norm_dist);
#endif
    if (norm_dist < best_dist) {
      best_dist = norm_dist;
      best_size = key_size;
    }
  }
#ifdef DEBUG
  printf("best size: %zu, best dist: %f\n", best_size, best_dist);
#endif
  uint8_t *key = malloc(best_size);
  for (size_t i = 0; i < best_size; i++) {
    uint8_t k = best_key_step(cyphertext, byte_len, i, best_size);
    key[i] = k;
  }
  print_key(key, best_size);
  printf("\ndecoded message:\n");
  uint8_t *decoded =
      xor_with_repeating_key(cyphertext, byte_len, key, best_size);
  if (fwrite(decoded, 1, byte_len, stdout) != byte_len) {
    printf("frwrite() failed\n");
    exit(1);
  }
  free(key);
  free(decoded);
  free(b64text);
  free(cyphertext);
  fclose(stream);
  return 0;
}
