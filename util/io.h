#include <stdint.h>
#include <stdio.h>

#define START_SZ 512
#define MAX_SZ (1UL << 24)
//^ 16MB buffer is largest we allow

/*
 * reads whole contents of a file as byte buffer.
 * allocates new buffer with size equal to file length in bytes.
 * writes a size of that buffer in '*len', then returns a pointer
 * to that new buffer. fseek() is used to determine file size,
 * so this won't work on streams lacking support for seek(), eg. pipes.
 * returns NULL and prints an error message if anything goes wrong!!
 */
uint8_t *get_file_contents(FILE *stream, size_t *len);

/*
 * reads all non-spaces characters from the stream (although
 * skips SPC, TAB, EOL) and returns a newly allocated
 * NULL-terminated string, or NULL if something goes wrong.
 */
char *read_non_space(FILE *stream);
