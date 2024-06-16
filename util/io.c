#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "io.h"

uint8_t *get_file_contents(FILE *stream, size_t *len) {
  assert(len != NULL && stream != NULL);
  size_t num_bytes;
  uint8_t *buffer;
  //jump to eof to determine size
  if (fseek(stream, 0, SEEK_END) == -1) {
    perror("fseek");
    return NULL;
  }
  long pos = ftell(stream);
  if (pos == -1) {
    perror("ftell");
    return NULL;
  } else
    num_bytes = (size_t) pos;
  //jump back to beginning
  if (fseek(stream, 0, SEEK_SET) == -1) {
    perror("fseek");
    return NULL;
  }
  buffer = malloc(num_bytes);
  if (buffer == NULL) {
    perror("malloc");
    return NULL;
  }
  if (fread(buffer, 1, num_bytes, stream) != num_bytes) {
    fprintf(stderr, "fread() failed\n");
    free(buffer);
    return NULL;
  }
  *len = num_bytes;
  return buffer;
}

char *read_non_space(FILE *stream) {
  int c, ix = 0;
  char *str = malloc(START_SZ); //this means we have START_SZ - 1 chars available
  size_t remaining_chars = START_SZ - 1, buf_sz = START_SZ;
  for (;;) {
    c = fgetc(stream);
    if (feof(stream))
      break;
    if (ferror(stream)) {
      perror("fgetc");
      return NULL;
    }
    if (isspace(c))
      continue;
    if (remaining_chars == 0) {
      remaining_chars = buf_sz - 1;
      if (buf_sz < MAX_SZ)
        buf_sz *= 2;
      else {
        fprintf(stderr, "input larger than %ld bytes (too large)\n", MAX_SZ);
          return NULL;
      }
      char *str_new = realloc(str, buf_sz);
      if (str_new == NULL) {
        free(str);
        return NULL;
      } else
        str = str_new;
    }
    //append char to buffer
    str[ix++] = c;
    remaining_chars--;
  }
  //hopefully we've allocated enough space to append NULL-byte...
  str[ix] = '\0';
  return str;
}
