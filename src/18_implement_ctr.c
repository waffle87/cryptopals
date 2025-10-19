// Implement CTR, the stream cipher mode
#include "aes.h"
#include "base64.h"

/*
 * the string:
 * L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==
 * ...decrypts to something approximating english in ctr mode, which is an aes
 block cipher mode that turns aes into a stream cipher, with the following
 parameters:
 * key=YELLOW SUBMARINE
 * nonce=0
 * format=64 bit unsigned little endian nonce,
 *        64 bit little endian block count (byte count / 16)
 * ctr mode is very simple. instead of encrypting the plaintext, ctr mode
 encrypts a running counter, producing a 16 byte block of keystream, which is
 xor'd against the plaintext. for instance, the first 16 byts of a message with
 these parameters:
 * keystream = AES("YELLOW
 SUBMARINE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
 * ...for the next 16 bytes:
 * keystream = AES("YELLOW
 SUBMARINE\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00")
 * ...and then:
 * keystream = AES("YELLOW
 SUBMARINE\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00")
 * ctr mode does not require padding; run you run out of plaintext, you just
 stop xor'ing keystream and stop generating keystream. decryption is identical
 to encryption. generate the same keystream, xor, and recover the plaintext.
 decrypt the string at the top of this function, then use your ctr function to
 encrypt and decrypt other things. this is the only block cipher mode that
 matters in good code. most modern cryptography relies on ctr mode to adapat
 block ciphers into stream ciphers, because most of what we want to encrypt is
 better described as a stream than a sequence of blocks. daneil bernstein once
 quipped to phil rogaway that cood cryptosystems don't need the "decrypt"
 transforms. constructions like ctr are what he was talking about
*/

int main(void) {
  char *input = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/"
                "kXX0KSvoOLSFQ==";
  char *key = "YELLOW SUBMARINE";
  char *decoded = NULL;
  int decoded_len = base64_decode(input, strlen(input), &decoded);
  uint64_t nonce = 0;
  struct bytes res = aes_ctr((unsigned char *)decoded, decoded_len,
                             (const unsigned char *)key, strlen(key), nonce);
  if (res.data) {
    printf("%s", res.data);
    free(res.data);
  }
  printf("\n");
  free(decoded);
  return 0;
}
