#include "file.h"

int read_file(const char *filename, char **res) {
  int file_len = 0;
  FILE *fptr = fopen(filename, "r");
  if (!fptr) {
    fprintf(stderr, "Error: %s (errno: %d)\n", strerror(errno), errno);
    return -1;
  }
  fseek(fptr, 0, SEEK_END);
  file_len = ftell(fptr);
  rewind(fptr);
  *res = (char *)malloc((file_len + 1) * sizeof(char));
  if (fread((*res), file_len, sizeof(char), fptr) != 1) {
    fprintf(stderr, "Error reading from file %s\n", filename);
    return -1;
  }
  fclose(fptr);
  return file_len;
}
