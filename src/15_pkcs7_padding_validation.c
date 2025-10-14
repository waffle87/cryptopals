// PKCS#7 padding validation
#include "aes.h"

/*
 * write a function that takes a plaintext, determines if it has valid pkcs#7
 * padding, and strips the padding off. the sring "ICE ICE BABY\x04\x04\x04\x04"
 * has valid padding, and produces the result "ICE ICE BABY". the string "ICE
 * ICE BABY\x05\x05\x05\x05" does not have valid padding, nor does "ICE ICE
 * BABY\x01\x02\x03\x04". if you are writing a language with exceptions, like
 * python, make your function throw an exception on bad padding.
 */

int main(void) {
  static char *valid = "ICE ICE BABY\x04\x04\x04\x04";
  static char *invalid1 = "ICE ICE BABY\x05\x05\x05\x05";
  static char *invalid2 = "ICE ICE BABY\x01\x02\x03\x04";
  struct bytes stripped = pkcs7_validate_strip((unsigned char *)valid, 16);
  if (stripped.data)
    printf("%s\n", stripped.data);
  else
    printf("Invalid padding\n");
  stripped = pkcs7_validate_strip((unsigned char *)invalid1, 16);
  if (stripped.data)
    printf("%s\n", stripped.data);
  else
    printf("Invalid padding\n");
  stripped = pkcs7_validate_strip((unsigned char *)invalid2, 16);
  if (stripped.data)
    printf("%s\n", stripped.data);
  else
    printf("Invalid padding\n");
  free(stripped.data);
  return 0;
}
