#include <ctype.h>
#include <math.h> //not part of c stdlib, so compile with -lm to link
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "base64.h"

//*** encoding ***//
static size_t base64_encoded_length(size_t bytes_len) {
  return (size_t)(4 * ceil(bytes_len / 3.0));
}

static char encode_char(sextet s) {
  char res;
  assert(s < 64);
  if (s < 26)
    //0..25 -> 'A'...'Z'
    res = 'A' + s;
  else if (s < 52)
    //26...51 -> 'a'...'z'
    res = 'a' + (s - 26);
  else if (s < 62)
    //52...61 -> '0'...'9'
    res = '0' + (s - 52);
  else if (s == 62)
    res = '+';
  else
    res = '/';
  return res;
}

static void encode_1byte(uint8_t byte1, char *encoded) {
  sextet s[2];
  s[0] = byte1 >> 2;
  s[1] = (byte1 & 0x03) << 4;
  *encoded++ = encode_char(s[0]);
  *encoded++ = encode_char(s[1]);
  *encoded++ = '=';
  *encoded = '=';
}

static void encode_2byte(uint8_t byte1, uint8_t byte2, char *encoded) {
  sextet s[3];
  s[0] = byte1 >> 2;
  s[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
  s[2] = (byte2 & 0x0F) << 2;
  for (int i = 0; i < 3; i++)
    *encoded++ = encode_char(s[i]);
  *encoded = '=';
}

static void encode_3byte(uint8_t byte1, uint8_t byte2, uint8_t byte3, char *encoded) {
  sextet s[4];
  s[0] = byte1 >> 2;
  s[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
  s[2] = ((byte2 & 0x0F) << 2) | (byte3 >> 6);
  s[3] = byte3 & 0x3F;
  for (int i = 0; i < 4; i++)
    *encoded++ = encode_char(s[i]);
}

char *base64_encode(uint8_t *from, size_t len) {
  long padded_bytes;
  size_t to_len;
  char *to, *curr;
  //first determine size of required result buffer
  //then malloc (with 1 extra byte for '\0')
  to_len = base64_encoded_length(len);
  to = (char*) malloc(to_len + 1);
  if (to == NULL)
    return NULL;
  //handle 3 bytes at a time,
  //converting into 4 sextets
  curr = to;
  for (unsigned long i = 0; i < len / 3; i++) {
    encode_3byte(*from, *(from + 1), *(from + 2), curr);
    from += 3;
    curr += 4;
  }
  //convert leftover bytes (if any)
  padded_bytes = len - 3 * (len / 3);
  if (padded_bytes == 1)
    encode_1byte(*from, curr);
  else if (padded_bytes == 2)
    encode_2byte(*from, *(from + 1), curr);
  //aformentioned zero-terminate
  to[to_len] = '\0';
  return to;
}

//*** decoding ***//
#define WRONG_CHAR 64

static uint8_t decode_char(char c) {
  uint8_t res;
  assert(isalnum(c) || c == '+' || c == '/');
  if (isupper(c))
    res = c - 'A';
  else if (islower(c))
    res = c - 'a' + 26;
  else if (isdigit(c))
    res = c - '0' + 52;
  else if (c == '+')
    res = 62;
  else if (c == '/')
    res = 63;
  else {
    fprintf(stderr, "unsupported char: %c\n", c);
    res = WRONG_CHAR;
  }
  return res;
}

static void decode_1byte(char *from, uint8_t *to) {
  //read/decode 2 sextets
  uint8_t s1 = decode_char(*from);
  uint8_t s2 = decode_char(*(from + 1));
  //check for error
  assert(s1 != WRONG_CHAR);
  assert(s2 != WRONG_CHAR);
  //combine 2 sextets (12 bits) into single byte
  uint8_t byte1 = (s1 << 2) | (s2 >> 4);
  *to++ = byte1;
}

static void decode_2byte(char *from, uint8_t *to) {
  //read/decode 3 sextets
  uint8_t s1 = decode_char(*from);
  uint8_t s2 = decode_char(*(from + 1));
  uint8_t s3 = decode_char(*(from + 2));
  //check for error
  assert(s1 != WRONG_CHAR);
  assert(s2 != WRONG_CHAR);
  assert(s3 != WRONG_CHAR);
  //combine 3 sextets (18 bits) into single byte
  uint8_t byte1 = (s1 << 2) | (s2 >> 4);
  uint8_t byte2 = (s2 << 4) | (s3 >> 2);
  *to++ = byte1;
  *to++ = byte1;
}

static void decode_3byte(char *from, uint8_t *to) {
  //read/decode 4 sextets
  uint8_t s1 = decode_char(*from);
  uint8_t s2 = decode_char(*(from + 1));
  uint8_t s3 = decode_char(*(from + 2));
  uint8_t s4 = decode_char(*(from + 3));
  //check for error
  assert(s1 != WRONG_CHAR);
  assert(s2 != WRONG_CHAR);
  assert(s3 != WRONG_CHAR);
  assert(s4 != WRONG_CHAR);
 //combine 4 sextets (24 bits) into single byte
  uint8_t byte1 = (s1 << 2) | (s2 >> 4);
  uint8_t byte2 = (s2 << 4) | (s3 >> 2);
  uint8_t byte3 = (s3 << 6) | s4;
  *to++ = byte1;
  *to++ = byte1;
  *to++ = byte3;
}

uint8_t *base64_decode(char *from, size_t *len) {
  assert(from != NULL && len != NULL);
  size_t text_len = strlen(from);
  //risky assumption data is properly padded...
  //so always use 4-character blocks
  assert(text_len % 4 == 0);
  //analyze padding...
  char last = from[text_len - 1], pre_last = from[text_len - 2];
  int last_block_len = 3;
  if (last == '=')
    last_block_len = (pre_last == '=') ? 1 : 2;
  //determine size of decoded bytes
  size_t bytes_len = (text_len / 4 - 1) * 3 + last_block_len;
  uint8_t *to = malloc(bytes_len);
  if (to == NULL) {
    perror("malloc");
    return NULL;
  }
  //read 4-bytes and write 3-bytes on each loop
  unsigned long i;
  for (i = 0; i < text_len / 4 - 1; i++)
    decode_3byte(&from[4 * i], &to[3 * i]);
  //handle last block (ideally with padding)
  if (last_block_len == 3)
    decode_3byte(&from[4 * i], &to[3 * i]);
  else if (last_block_len == 1)
    decode_1byte(&from[4 * i], &to[3 * i]);
  else if (last_block_len == 2)
    decode_2byte(&from[4 * i], &to[3 * i]);
  *len = bytes_len;
  return to;
}
