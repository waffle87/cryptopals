#include "base64.h"

static const unsigned char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int mod_table[] = {0, 2, 1};

int base64_encode(const char *base64, size_t base64_len, char **res) {
  size_t res_len = 4 * ((base64_len + 2) / 3);
  int padding = mod_table[base64_len % 3];
  *res = (char *)malloc((res_len + 1) * sizeof(char));
  for (size_t i = 0, j = 0; i < base64_len;) {
    int octet_a = i < base64_len ? base64[i++] : 0;
    int octet_b = i < base64_len ? base64[i++] : 0;
    int octet_c = i < base64_len ? base64[i++] : 0;
    int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
    (*res)[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
    (*res)[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
    (*res)[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
    (*res)[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
  }
  for (int i = 0; i < padding; i++)
    (*res)[res_len - 1 - i] = '=';
  return res_len;
}

int base64_decode(const char *base64, size_t base64_len, char **res) {
  unsigned char dtable[256], in[4], block[4], tmp;
  char *pos;
  memset(dtable, 0x80, sizeof(dtable));
  for (size_t i = 0; i < sizeof(base64_table); i++)
    dtable[base64_table[i]] = i;
  dtable['='] = 0;
  size_t cnt = 0;
  for (size_t i = 0; i < base64_len; i++)
    if (dtable[(int)base64[i]] != 0x80)
      cnt++;
  if (cnt % 4)
    return -1;
  size_t res_len = cnt / 4 * 3;
  pos = *res = (char *)malloc(cnt * sizeof(char));
  cnt = 0;
  for (size_t i = 0; i < base64_len; i++) {
    tmp = dtable[(int)base64[i]];
    if (tmp == 0x80)
      continue;
    in[cnt] = base64[i];
    block[cnt] = tmp;
    cnt++;
    if (cnt == 4) {
      *pos++ = (block[0] << 2) | (block[1] >> 4);
      *pos++ = (block[1] << 4) | (block[2] >> 2);
      *pos++ = (block[2] << 6) | block[3];
      cnt = 0;
    }
  }
  if (pos > *res) {
    if (in[2] == '=')
      res_len -= 2;
    else if (in[3] == '=')
      res_len--;
  }
  return res_len;
}
