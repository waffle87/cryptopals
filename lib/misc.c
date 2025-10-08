#include "misc.h"

bool contains(const unsigned char *data, size_t data_len, const char *expr) {
  size_t expr_len = strlen(expr);
  if (expr_len > data_len)
    return false;
  for (size_t i = 0; i < data_len - expr_len; i++)
    if (!memcmp(data + i, expr, expr_len))
      return true;
  return false;
}
