#include <stdint.h>
#include <stdio.h>
/*
 * accepts null-terminated hex-string (ie. "4AE6" or "4ae6")
 * which should contain even number of hex digits.
 * return corresponding malloc'd byte buffer
 * whose length is half of the input string's
 */
static uint8_t char_to_four_bits(char c);
static char four_bits_to_char(uint8_t b);
uint8_t *read_hex(char *hex_str, size_t hex_len);
char *write_hex(uint8_t *bytes, size_t bytes_len);
