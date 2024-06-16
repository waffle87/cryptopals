#include <stdio.h>
#include <inttypes.h>

/*
 * return bitwise xor of two byte buffers of the same length
 * the resulting byte buffer is malloc'd (so free() it!!!)
 */
uint8_t *xor(uint8_t *b1, uint8_t *b2, size_t len);

/*
 * return bitwise xor of two buffers represented as hex strings
 * result will be encoded as hex string. the resuling string
 * is malloc'd (so free() it!!!)
 */
char *xor_hex(char *s1, char *s2);

/*
 * returns bitwise xor of a single byte key and provided byte message
 * the resuling byte buffer is malloc'd (so free() it!!!)
 */
uint8_t *xor_with_byte_key(uint8_t *msg, uint8_t key, size_t len);

/*
 * returns bitwise xor of a single byte key and provided byte message
 * the resuling byte buffer is malloc'd (so free() it!!!)
 */
uint8_t *xor_with_repeating_key(uint8_t *msg, size_t msg_len, uint8_t *key, size_t key_len);
