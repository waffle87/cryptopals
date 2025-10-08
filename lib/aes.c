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
  if (key_len != 16 && key_len != 24 && key_len != 32)
    return (struct bytes){NULL, 0};
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  const EVP_CIPHER *cipher;
  switch (key_len) {
  case 16:
    cipher = EVP_aes_128_ecb();
    break;
  case 24:
    cipher = EVP_aes_192_ecb();
    break;
  case 32:
    cipher = EVP_aes_256_ecb();
    break;
  }
  if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, NULL) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0);
  struct bytes padded = pkcs7(data, data_len, AES_BLOCK_SIZE);
  if (!padded.data) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  unsigned char *ciphertext =
      (unsigned char *)malloc(padded.len * sizeof(unsigned char));
  int len;
  if (EVP_EncryptUpdate(ctx, ciphertext, &len, padded.data, padded.len) != 1) {
    free(padded.data);
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  int ciphertext_len = len;
  if (EVP_EncryptFinal(ctx, ciphertext + len, &len) != 1) {
    free(padded.data);
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  ciphertext_len += len;
  free(padded.data);
  EVP_CIPHER_CTX_free(ctx);
  return (struct bytes){ciphertext, ciphertext_len};
}

struct bytes aes_decrypt_ecb(const unsigned char *data, size_t data_len,
                             const unsigned char *key, size_t key_len) {
  if (key_len != 16 && key_len != 24 && key_len != 32)
    return (struct bytes){NULL, 0};
  if (data_len == 0 || data_len % AES_BLOCK_SIZE)
    return (struct bytes){NULL, 0};
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  const EVP_CIPHER *cipher;
  switch (key_len) {
  case 16:
    cipher = EVP_aes_128_ecb();
    break;
  case 24:
    cipher = EVP_aes_192_ecb();
    break;
  case 32:
    cipher = EVP_aes_256_ecb();
    break;
  }
  if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, NULL) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0);
  unsigned char *plaintext =
      (unsigned char *)malloc(data_len * sizeof(unsigned char));
  int len;
  if (EVP_DecryptUpdate(ctx, plaintext, &len, data, data_len) != 1) {
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  int plaintext_len = len;
  if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);
  struct bytes unpadded = pkcs7_strip(plaintext, plaintext_len);
  free(plaintext);
  return unpadded;
}

struct bytes aes_encrypt_cbc(char *data, const size_t data_len,
                             const unsigned char *key, const size_t key_len,
                             unsigned char *iv) {
  if (key_len != 16 && key_len != 24 && key_len != 32)
    return (struct bytes){NULL, 0};
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  const EVP_CIPHER *cipher;
  switch (key_len) {
  case 16:
    EVP_aes_128_cbc();
    break;
  case 24:
    EVP_aes_192_cbc();
    break;
  case 32:
    EVP_aes_256_cbc();
    break;
  }
  if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  struct bytes padded = pkcs7(data, data_len, AES_BLOCK_SIZE);
  if (!padded.data) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0);
  unsigned char *ciphertext =
      (unsigned char *)malloc(padded.len * sizeof(unsigned char));
  int len;
  if (EVP_EncryptUpdate(ctx, ciphertext, &len, padded.data, padded.len) != 1) {
    free(padded.data);
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  int ciphertext_len = len;
  if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
    free(padded.data);
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  ciphertext_len += len;
  free(padded.data);
  EVP_CIPHER_CTX_free(ctx);
  return (struct bytes){ciphertext, ciphertext_len};
}

struct bytes aes_decrypt_cbc(unsigned char *data, size_t data_len,
                             const unsigned char *key, size_t key_len,
                             unsigned char *iv) {
  if (key_len != 16 && key_len != 24 && key_len != 32)
    return (struct bytes){NULL, 0};
  if (!data_len || data_len % AES_BLOCK_SIZE)
    return (struct bytes){NULL, 0};
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  const EVP_CIPHER *cipher;
  switch (key_len) {
  case 16:
    cipher = EVP_aes_128_cbc();
    break;
  case 24:
    cipher = EVP_aes_192_cbc();
    break;
  case 32:
    cipher = EVP_aes_256_cbc();
    break;
  }
  if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0);
  unsigned char *plaintext =
      (unsigned char *)malloc(data_len * sizeof(unsigned char));
  int len;
  if (EVP_DecryptUpdate(ctx, plaintext, &len, data, data_len) != 1) {
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  int plaintext_len = len;
  if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);
  struct bytes res = pkcs7_validate_strip(plaintext, plaintext_len);
  free(plaintext);
  return res;
}
