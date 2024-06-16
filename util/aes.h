#include <openssl/evp.h>

//initializes EVP context
EVP_CIPHER_CTX *evp_init();

//cleans up EVP context
void evp_cleanup(EVP_CIPHER_CTX *ctx);

//error handling function
void handle_evp_errors();

/*
 * encrypts/decrypts AES-encoded data in ECB mode. takes byte string 'in',
 * its length, & the key (128bit). writes output to 'out' and
 * returns its size or -1 if anything goes wrong.
 * handle_evp_errors() can be used to print error messages.
 */
int evp_ecb_encrypt(EVP_CIPHER_CTX *ctx, unsigned char *in, int in_len, unsigned char *key, unsigned char *out);
int evp_ecb_decrypt(EVP_CIPHER_CTX *ctx, unsigned char *in, int in_len, unsigned char *key, unsigned char *out);

/*
 * encrypts/decrypts AES-encoded data in CBC mode using low-level OpenSSL
 * interface. takes byte string 'in', its length, the key, & 'iv'
 * (128bit). writes output to 'out' and returns its size of -1
 * if anything goes wrong. handle_evp_errors() can be used
 * to pint error messages. CBC also handles padding
 */
int cbc_encrypt(unsigned char *in, int in_len, unsigned char *key, unsigned char *iv, unsigned char *out);
int cbc_decrypt(unsigned char *in, int in_len, unsigned char *key, unsigned char *iv, unsigned char *out);

/*
 * encrypts/decrypts AES-encoded data in CTR mode using low-level
 * OpenSSL interface. takes byte string 'in' its length, the key
 * & the nonce. writes output to 'out' (its length should be the
 * same as 'in' length). returns 0 on error, 1 on success.
 * encryption and decryption methods are identical.
 * no padding is made, as it is not needed.
 */
int ctr_encrypt(unsigned char *in, int in_len, unsigned char *key, uint64_t nonce, unsigned char *out);
int ctr_decrypt(unsigned char *in, int in_len, unsigned char *key, uint64_t nonce, unsigned char *out);
