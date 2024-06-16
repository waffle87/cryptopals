#include <stdint.h>
#include <stddef.h>

//returns number of 1-bits set in a byte.
//(so called "population count".. see wikipedia)
size_t pop_count(uint8_t byte);

//takes two NULL-terminated strings of same length.
//returns number of differing bits.
size_t hamming_distance(char *s1, char *s2);

//takes two byte arrays of same length & their length
//returns number of differing bits.
size_t hamming_distance_bytes(uint8_t *b1, uint8_t *b2, size_t len);
