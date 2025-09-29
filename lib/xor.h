#pragma once

#include <limits.h>
#include <stdlib.h>

int xor (const unsigned char *s1, const size_t s1_len, const unsigned char *s2,
         const size_t s2_len, unsigned char **res);

char find_single_byte_xor(const unsigned char *input, const size_t input_len,
                          int (*score_function)(const unsigned char *,
                                                const size_t));

int hamming_distance(const unsigned char *s1, const size_t s1_len,
                     const unsigned char *s2, const size_t s2_len);

int find_xor_key_len(const unsigned char *input, const size_t input_len,
                     const size_t min_key_len, const size_t max_key_len);
