#pragma once

#include <stdlib.h>
#include <string.h>

int base64_encode(const char *base64, size_t base64_len, char **res);
int base64_decode(const char *base64, size_t base64_len, char **res);
