#pragma once

#include <stdlib.h>
#include <string.h>

int hex_to_int(char c);
int hex_to_ascii(const char *hex, char **res);
char hex_digit(unsigned int n);
int ascii_to_hex(const char *ascii, char **res);
int to_hex(const unsigned char *data, size_t data_len, char **res);
