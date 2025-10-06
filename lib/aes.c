#include "aes.h"

int next_block(int length, size_t block_size) {
  return (length + block_size) & ~(block_size - 1);
}

struct bytes pkcs7(const char *data, const size_t data_len, size_t block_size) {
  size_t res_len = next_block(data_len, block_size);
  unsigned char *res = (unsigned char *)malloc(res_len * sizeof(unsigned char));
  unsigned char padding = res_len - data_len;
  memcpy(res, data, data_len);
  for (size_t i = data_len; i < res_len; i++)
    res[i] = padding;
  return (struct bytes){res, res_len};
}

struct bytes pkcs7_strip(unsigned char *data, size_t data_len) {
  size_t padding = data[data_len - 1], res_len = data_len - padding;
  unsigned char *res = (unsigned char *)malloc(res_len * sizeof(unsigned char));
  memcpy(res, data, res_len);
  return (struct bytes){res, res_len};
}

bool pkcs7_validate(unsigned char *data, size_t data_len) {
  size_t padding = data[data_len - 1];
  if (!padding || padding > data_len)
    return false;
  for (size_t i = 1; i < padding; i++)
    if (padding != data[data_len - 1 - i])
      return false;
  return true;
}

struct bytes pkcs7_validate_strip(unsigned char *data, size_t data_len) {
  struct bytes res = {NULL, 0};
  if (pkcs7_validate(data, data_len))
    res = pkcs7_strip(data, data_len);
  return res;
}

struct bytes aes_encrypt_ecb(char *data, const size_t data_len,
                             const unsigned char *key, const size_t key_len) {
  AES_KEY aes_key;
  AES_set_encrypt_key(key, key_len * 8, &aes_key);
  struct bytes padded = pkcs7(data, data_len, AES_BLOCK_SIZE);
  unsigned char *res =
      (unsigned char *)calloc(padded.len, sizeof(unsigned char));
  for (size_t i = 0; i < padded.len; i += AES_BLOCK_SIZE)
    AES_ecb_encrypt(padded.data + i, res + i, &aes_key, AES_ENCRYPT);
  free(padded.data);
  return (struct bytes){res, padded.len};
}

struct bytes aes_decrypt_ecb(const unsigned char *data, size_t data_len,
                             const unsigned char *key, size_t key_len) {
  struct bytes res = {NULL, 0};
  unsigned char *decrypted = calloc(data_len + 1, sizeof(unsigned char));
  AES_KEY aes_key;
  AES_set_decrypt_key(key, key_len * 8, &aes_key);
  for (size_t i = 0; i < data_len; i += AES_BLOCK_SIZE)
    AES_ecb_encrypt(data + i, decrypted + i, &aes_key, AES_DECRYPT);
  if (data_len)
    res = pkcs7_strip(decrypted, data_len);
  free(decrypted);
  return res;
}
