// CBC bitflipping attacks
#include "aes.h"
#include "misc.h"
#include "random.h"
/*
 * generate a random aes key. combine your padding code and cbc code to write
 * tow functions. the first function should take an arbitrary input string.
 * prepend the string: "comment1=cooking%20MCs;userdata=" and append the string:
 * ";comment2=%20like%20a%20of%20bacon". the function should quote out the ";"
 * and "=" characters. the function should then pad out the input to the 16 byte
 * aes block length and encrypt it under the random aes key. the second function
 * should decrypt the string and look for the characters ";admin=true;" (or
 * equivalently, decrypt, split the string on ";", convert each resulting string
 * into 2-tuples, and look for the "admin" tuple). return true of false based on
 * whether the string exists. if you've written the first function properly, it
 * should not be possible to provide user input to it that will generate the
 * string the second function is looking for. we'll have to break the crypto to
 * do that. instead, modify the ciphertext (without knowledge of the aes key) to
 * accomplish this. you're relying on the fact that in cbc mode, a 1-bit error
 * in a ciphertext block completely scrambles the block the error occurs in and
 * produces the identical 1-bit error(/edit) in the next ciphertext block.
 * before you implement this attack, answer this question: why does cbc mode
 * have this property?
 */

const unsigned char *key;
unsigned char iv[16] = {0};

char *strip_special_chars(char *input, size_t input_len) {
  char *res = (char *)malloc(input_len * sizeof(char));
  for (size_t i = 0; i < input_len; i++)
    if (input[i] == ';' || input[i] == '=')
      res[i] = ' ';
    else
      res[i] = input[i];
  return res;
}

struct bytes strip_encrypt_cbc(char *data, size_t data_len) {
  static char *prefix = "comment1=cooking%20MCs;userdata=";
  static char *suffix = ";comment2=%20like%20a%20pound%20of%20bacon";
  size_t prefix_len = strlen(prefix);
  size_t suffix_len = strlen(suffix);
  size_t input_len = data_len + prefix_len + suffix_len;
  char *input = (char *)malloc(input_len * sizeof(char));
  memcpy(input, prefix, prefix_len);
  memcpy(input + prefix_len, data, data_len);
  memcpy(input + prefix_len + data_len, suffix, suffix_len);
  char *stripped = strip_special_chars(input, input_len);
  struct bytes res = aes_encrypt_cbc(stripped, input_len, key, 16, iv);
  free(input);
  free(stripped);
  return res;
}

int main(void) {
  srand(time(NULL));
  key = random_bytes(16);
  char data[32] = {0};
  memcpy(data + 16, ":admin<true:", 12);
  struct bytes encrypted = strip_encrypt_cbc(data, sizeof(data));
  encrypted.data[32] ^= 0x01;
  encrypted.data[32 + 6] ^= 0x01;
  encrypted.data[32 + 11] ^= 0x01;
  struct bytes decrypted =
      aes_decrypt_cbc(encrypted.data, encrypted.len, key, 16, iv);
  char *target = ";admin=true;";
  if (contains(decrypted.data, decrypted.len, target))
    printf("Success: %s in decrypted data\n", target);
  else
    printf("%.*s did not include %s\n", (int)decrypted.len, decrypted.data,
           target);
  free(encrypted.data);
  free(decrypted.data);
  return 0;
}
