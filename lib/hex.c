#include "hex.h"

int hex_to_int(char c) {
  int first = c / 16 - 3;
  int second = c % 16;
  int res = first * 10 + second;
  if (res > 9)
    res--;
  if (res > 29)
    res -= 20;
  return res;
}

int hex_to_ascii(const char *hex, char **res) {
  size_t hex_len = strlen(hex), res_len = hex_len / 2, i;
  *res = (char *)malloc((res_len + 1) * sizeof(char));
  char buf = 0;
  for (i = 0; i < hex_len; i++)
    if (i % 2) {
      int high = hex_to_int(buf) * 16;
      int low = hex_to_int(hex[i]);
      (*res)[i / 2] = high + low;
    } else
      buf = hex[i];
  (*res)[i] = 0;
  return res_len;
}

char hex_digit(unsigned int n) { return n < 10 ? n + '0' : (n - 10) + 'a'; }

int ascii_to_hex(const char *ascii, char **res) {
  size_t ascii_len = strlen((char *)ascii), res_len = ascii_len * 2, i;
  *res = (char *)malloc((res_len + 1) * sizeof(char));
  for (i = 0; i < ascii_len; i++) {
    (*res)[i * 2] = hex_digit(ascii[i] / 0x10);
    (*res)[i * 2 + 1] = hex_digit(ascii[i] % 0x10);
  }
  (*res)[i * 2] = 0;
  return res_len;
}

int to_hex(const unsigned char *data, size_t data_len, char **res) {
  size_t res_len = data_len * 2, i;
  *res = (char *)malloc((res_len + 1) * sizeof(char));
  for (i = 0; i < data_len; i++) {
    (*res)[i * 2] = hex_digit(data[i] / 0x10);
    (*res)[i * 2 + 1] = hex_digit(data[i] % 0x10);
  }
  (*res)[i * 2] = 0;
  return res_len;
}
