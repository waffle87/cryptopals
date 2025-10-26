// Break "random access read/write" AES CTR
#include "aes.h"
#include "base64.h"
#include "file.h"
#include "random.h"

/*
 * back to ctr. encrypt the recovered plaintext from [data/25.txt] (the ecb
 * exercise) under ctr with a random key (for this exercise the key should be
 * unknown to you, but hold onto it). now, write the code that allows you to
 * seek into the ciphertext, decrypt, and re-encrypt with different plaintext.
 * expose this as a function like 'edit(ciphertext, key, offset, newtext)'.
 * imagine the "edit" function was exposed to attackers by means of an api call
 * that didn't reveal the key or the original plaintext; the attacker has the
 * ciphertext and controls the offset and "new text". recover the original
 * plaintext. food for thought: a folkloric supposed benefit of ctr mode is the
 * ability to easily "seek forward" into the ciphertext; to access byte n of the
 * ciphertext, all you need to be able to do is generate byte 'n' of the
 * keystream. imagine if you'd relied on that advice, to say, encrypt a disk.
 */

struct bytes edit(struct bytes ciphertext, const unsigned char *key, int offset,
                  char *newtext, size_t newtext_len) {
  struct bytes plain =
      aes_ctr(ciphertext.data, ciphertext.len, key, AES_BLOCK_SIZE, 0);
  struct bytes prefix = {plain.data, offset};
  struct bytes suffix = {plain.data + offset + newtext_len,
                         plain.len - offset - newtext_len};
  char *data = (char *)malloc(ciphertext.len * sizeof(char));
  memcpy(data, prefix.data, prefix.len);
  memcpy(data + prefix.len, newtext, newtext_len);
  memcpy(data + prefix.len + newtext_len, suffix.data, suffix.len);
  struct bytes res = aes_ctr((const unsigned char *)data, ciphertext.len, key,
                             AES_BLOCK_SIZE, 0);
  free(plain.data);
  free(data);
  return res;
}

int main(void) {
  srand(time(NULL));
  const unsigned char *rand_key = random_bytes(AES_BLOCK_SIZE);
  const uint64_t nonce = 0;
  char *input = NULL, *decoded = NULL;
  int file_len = read_file("data/25.txt", &input);
  int decoded_len = base64_decode(input, file_len, &decoded);
  const char *key = "YELLOW SUBMARINE";
  struct bytes res1 = aes_decrypt_ecb((unsigned char *)decoded, decoded_len,
                                      (const unsigned char *)key, strlen(key));
  struct bytes decrypted = aes_ctr(res1.data, res1.len, rand_key, 16, nonce);
  char *zeros = (char *)calloc(decrypted.len, sizeof(char));
  struct bytes keystream = edit(decrypted, rand_key, 0, zeros, decrypted.len);
  unsigned char *recovered;
  int res2 = xor(keystream.data, keystream.len, decrypted.data, decrypted.len,
                 &recovered);
  printf("%s\n", recovered);
  free(decoded);
  free(res1.data);
  free(decrypted.data);
  free(keystream.data);
  return 0;
}
