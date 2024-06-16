#include <stdint.h>
#include <stddef.h>

typedef uint8_t sextet;
static size_t base64_encoded_length(size_t bytes_len);
static char encode_char(sextet s);
static void encode_1byte(uint8_t byte1, char *encoded);
static void encode_2byte(uint8_t byte1, uint8_t byte2, char *encoded);
static void encode_3byte(uint8_t byte1, uint8_t byte2, uint8_t byte3, char *encoded);
/*
 * encodes to BASE64, returns pointer to newly allocated
 * zero-terminated string or NULL if error.
 * make sure to free() string!!
 */
char *base64_encode(uint8_t *from, size_t len);
static uint8_t decode_char(char c);
static void decode_1byte(char *from, uint8_t *to);
static void decode_2byte(char *from, uint8_t *to);
static void decode_3byte(char *from, uint8_t *to);
/*
 * accept a NULL-terminated string, decode it as BASE64,
 * returns a pointer to newly allocated byte array or NULL
 * if error. writes the size of array to *len.
 */
uint8_t *base64_decode(char *from, size_t *len);
