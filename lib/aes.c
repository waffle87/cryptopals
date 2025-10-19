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
  if (!data_len)
    return (struct bytes){NULL, 0};
  size_t padding = data[data_len - 1];
  if (!padding || padding > data_len) {
    unsigned char *res =
        (unsigned char *)malloc(data_len * sizeof(unsigned char));
    memcpy(res, data, data_len);
    return (struct bytes){res, data_len};
  }
  for (size_t i = data_len - padding; i < data_len; i++)
    if (data[i] != padding) {
      unsigned char *res =
          (unsigned char *)malloc(data_len * sizeof(unsigned char));
      memcpy(res, data, data_len);
      return (struct bytes){res, data_len};
    }
  size_t res_len = data_len - padding;
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
  default:
    return (struct bytes){NULL, 0};
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
  default:
    return (struct bytes){NULL, 0};
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
  default:
    return (struct bytes){NULL, 0};
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
  default:
    return (struct bytes){NULL, 0};
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

char *aes_oracle(unsigned char *encoded, size_t block_size) {
  if (!memcmp(encoded + block_size, encoded + block_size * 2, block_size))
    return "ECB";
  else
    return "CBC";
}

union aes_ctr_counter {
  unsigned char buf[AES_BLOCK_SIZE];
  struct nonce_counter {
    uint64_t nonce;
    uint64_t counter;
  } nc;
};

struct bytes aes_ctr(const unsigned char *data, size_t data_len,
                     const unsigned char *key, size_t key_len,
                     const uint64_t nonce) {
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
  default:
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, NULL) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return (struct bytes){NULL, 0};
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0);
  unsigned char *decrypted =
      (unsigned char *)calloc(data_len + 1, sizeof(unsigned char));
  unsigned char keystream[AES_BLOCK_SIZE];
  union aes_ctr_counter ctr_counter = {.nc = {.nonce = nonce, .counter = 0}};
  for (size_t count = 0; count < data_len; count += AES_BLOCK_SIZE) {
    size_t block_len =
        (data_len - count < AES_BLOCK_SIZE) ? data_len - count : AES_BLOCK_SIZE;
    ctr_counter.nc.counter = count / AES_BLOCK_SIZE;
    int tmp_len = 0;
    if (EVP_EncryptUpdate(ctx, keystream, &tmp_len, ctr_counter.buf,
                          AES_BLOCK_SIZE) != 1) {
      free(decrypted);
      EVP_CIPHER_CTX_free(ctx);
      return (struct bytes){NULL, 0};
    }
    for (size_t i = 0; i < block_len; i++)
      decrypted[count + i] = data[count + i] ^ keystream[i];
  }
  EVP_CIPHER_CTX_free(ctx);
  return (struct bytes){decrypted, data_len};
}
