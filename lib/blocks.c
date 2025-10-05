#include "blocks.h"

int block_transpose(const unsigned char *str, const int str_len,
                    const int num_blocks, unsigned char ***blocks) {
  int block_len = str_len / num_blocks;
  *blocks = (unsigned char **)malloc(num_blocks * sizeof(char *));
  for (int i = 0; i < num_blocks; i++) {
    (*blocks)[i] = (unsigned char *)malloc(block_len * sizeof(char));
    for (int j = 0; j < block_len; j++)
      (*blocks)[i][j] = str[i + j * num_blocks];
  }
  return block_len;
}

int chunk(const unsigned char *str, const int str_len, const int block_len,
          unsigned char ***blocks) {
  int num_blocks = str_len / block_len;
  *blocks = (unsigned char **)malloc(num_blocks * sizeof(char *));
  for (int i = 0; i < num_blocks; i++) {
    (*blocks)[i] = (unsigned char *)malloc(block_len * sizeof(char));
    for (int j = 0; j < block_len; j++)
      (*blocks)[i][j] = str[i * block_len + j];
  }
  return num_blocks;
}
