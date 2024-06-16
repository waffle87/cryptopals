#include <stdlib.h>
#include <inttypes.h>

size_t calc_padding_len(size_t data_len, size_t block_len);

/*
 * accepts a byte string, its length and 'block_len'.
 * pads the byte string, allocates a new buffer to contain
 * the string & the padding, copies data, appends padding
 * bytes, and writes the result to '*padded_data'.
 * returns size of the padded block or -1 if error.
*/
ssize_t add_padding(uint8_t *data, size_t data_len, size_t block_len, uint8_t **padded_data);

/*
 * accepts a byte string and its length. determines how much padding
 * should be added to the string and returns number of bytes to be
 * trimmed from the end of string, or -1 if error.
*/
ssize_t padding_len(uint8_t *padded_data, size_t data_len);
