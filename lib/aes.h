#pragma once

#include "xor.h"
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct bytes {
  unsigned char *data;
  size_t len;
};

int next_block(int length, size_t block_size);

struct bytes pkcs7(const char *data, const size_t data_len, size_t block_size);
struct bytes pkcs7_strip(unsigned char *data, size_t data_len);
bool pkcs7_validate(unsigned char *data, size_t data_len);
struct bytes pkcs7_validate_strip(unsigned char *data, size_t data_len);

struct bytes aes_encrypt_ecb(char *data, const size_t data_len,
                             const unsigned char *key, const size_t key_len);
struct bytes aes_decrypt_ecb(const unsigned char *data, size_t data_len,
                             const unsigned char *key, size_t key_len);
struct bytes aes_encrypt_cbc(char *data, const size_t data_len,
                             const unsigned char *key, const size_t key_len,
                             unsigned char *iv);
struct bytes aes_decrypt_cbc(unsigned char *data, size_t data_len,
                             const unsigned char *key, size_t key_len,
                             unsigned char *iv);
char *aes_oracle(unsigned char *encoded, size_t block_size);

struct bytes aes_ctr(const unsigned char *data, size_t data_len,
                     const unsigned char *key, size_t key_len,
                     const uint64_t nonce);
