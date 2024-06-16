#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "hex.h"
#include "xor.h"

uint8_t *xor(uint8_t *b1, uint8_t *b2, size_t len) {
  uint8_t *res = malloc(len);
  if (res == NULL) {
    printf("out of bounds\n");
    return NULL;
  }
  for (uint i = 0; i < len; i++)
    res[i] = b1[i] ^ b2[i];
  return res;
}

char *xor_hex(char *s1, char *s2) {
  assert(strlen(s1) == strlen(s2));
  size_t hex_len = strlen(s1), bytes_len = hex_len / 2;
  uint8_t *b1 = read_hex(s1, hex_len);
  uint8_t *b2 = read_hex(s2, hex_len);
  uint8_t *res = xor(b1, b2, bytes_len);
  return write_hex(res, bytes_len);
}

uint8_t *xor_with_byte_key(uint8_t *msg, uint8_t key, size_t len) {
  return xor_with_repeating_key(msg, len, &key, 1);
}

uint8_t *xor_with_repeating_key(uint8_t *msg, size_t msg_len, uint8_t *key, size_t key_len) {
  uint8_t *res = malloc(msg_len);
  if (res == NULL) {
    printf("out of bounds\n");
    return NULL;
  }
  uint j = 0;
  for (uint i = 0; i < msg_len; i++) {
    res[i] = msg[i] ^ key[j];
    j = (j + 1) % key_len;
  }
  return res;
}
