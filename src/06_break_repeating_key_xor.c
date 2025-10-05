// Break repeating-key XOR
#include "base64.h"
#include "blocks.h"
#include "file.h"
#include "stats.h"
#include "xor.h"

/*
 * this challenge isn't conceptually hard, but it involves actual error-prone
 * coding. the other challenges in this set are there to bring you up to speed.
 * this one is there to qualify you. if you can do this one, you're probably
 * just fine up to set 6.
 * [data/06.txt] has been base64'd after being encrypted with repeating key xor.
 * decrypt it. here's how:
 * 1. let KEYSIZE be the guessed length of the key; try values from 2 to
 * (say) 40.
 * 2. write a function to compute the edit distance/hamming distance between two
 * strings. the hamming distance is just the numnber of differing bits. the
 * distance between: this is a test and: wokka wokka!!! is 37. make sure your
 * code agrees before you proceed.
 * 3. for each KEYSIZE, take the first KEYSIZE worth of bytes, and the second
 * KEYSIZE worth of bytes, and find the edit distance between them. normalise
 * this result by dividing it by KEYSIZE
 * 4. the KEYSIZE with the smallest normalised edit distance is probably the
 * key. you could proceed perhaps with the smallest 2-3 KEYSIZE values. or take
 * 4 KEYSIZE blocks instead of 2 and average the distances
 * 5. now that you probably know the KEYSIZE: break the ciphertext into blocks
 * of KEYSIZE length
 * 6. now transpose the blocks: make a block that is the first byte of every
 * block, and a block that is the second byte of every block, adnd so on
 * 7. solve each block as if it was a single-character xor. you already have
 * code to do this. for each block, the single-byte xor key that procudes the
 * best looking histogram is the repeating key xor key byte for that block. put
 * them together and you have the key.
 * this code is going to turn out to be surprisingly useful later on. breakting
 * repeating-key xor ("vigenere") statistically is obviously an academic
 * exercise, a "crypto 101" thing. but more people "know how" to break it than
 * can actually break it, and a similar technique breaks something much more
 * important.
 */

int main(void) {
  char *input = NULL, *decoded = NULL;
  unsigned char *xord = NULL, **blocks = NULL;
  int file_len = read_file("data/06.txt", &input);
  size_t decoded_len = base64_decode(input, file_len, &decoded);
  size_t key_len =
      find_xor_key_len((unsigned char *)decoded, decoded_len, 2, 40);
  unsigned char *xor_key = (unsigned char *)malloc(key_len * sizeof(char));
  block_transpose((unsigned char *)decoded, decoded_len, key_len, &blocks);
  size_t i;
  for (i = 0; i < key_len; i++) {
    xor_key[i] = find_single_byte_xor(blocks[i], key_len, frequency_score);
    printf("%c", xor_key[i]);
  }
  xor_key[i] = 0;
  printf("\n");
  int res = xor((unsigned char *)decoded, decoded_len, xor_key, key_len, &xord);
  if (res != -1)
    printf("%s\n", xord);
  free(blocks);
  free(input);
  free(xord);
  free(xor_key);
  return 0;
}
