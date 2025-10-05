#pragma once

#include <stdlib.h>

int block_transpose(const unsigned char *str, const int str_len,
                    const int num_blocks, unsigned char ***blocks);
int chunk(const unsigned char *str, const int str_len, const int block_len,
          unsigned char ***blocks);
