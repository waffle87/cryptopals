// ECB cut-and-paste
#include "aes.h"
#include "random.h"

/*
 * write a k=v parsing routine, as if for a structured cookie. the routine
 * should take: foo=bar&baz=qux&zap=zazzle
 * ... and produce:
 * {
 *   foo: 'bar',
 *   baz: 'qux',
 *   zap: 'zazzle'
 * }
 * (you know, the object; i don't care if you convert it to json). now write a
 * function that encodes a user profile in that format, given an email address.
 * you should have something like: profile_for("foo@bar.com")
 * ... and it should produce:
 * {
 *   email: 'foo@bar.com',
 *   uid: 10,
 *   role: 'user'
 * }
 * ... encoded as:
 * email=foo@bar.com&uid=10&role=user
 * your 'profile_for' function should not allow encoding metacharacters (& and
 * =). eat them, quote them, whatever you want to do, but don't let people set
 * their email address to "foo@bar.com&role=admin". now, two more each
 * functions. generate a random aes key, then: a. encrypt the encoded user
 * profile under the key; "provide" that to the "attacker" b. decrypt the
 * encoded user profile and parse it using only the user input to
 * 'profile_for()' (as an oracle to generate "valud" ciphertexts) and the
 * ciphertexts themselves, make a 'role=admin' profile.
 */

unsigned char aes_key[16];

void replace_chars(char *s, char find, char replace) {
  for (int i = 0; s[i]; i++)
    if (s[i] == find)
      s[i] = replace;
}

void kv_pairs(const char *cookie) {
  char *copy = strdup(cookie);
  char *token = strtok(copy, "&");
  printf("{\n");
  while (token) {
    char *equals = strchr(token, '=');
    if (equals) {
      *equals = '\0';
      printf("  %s: %s\n", token, equals + 1);
    }
    token = strtok(NULL, "&");
  }
  printf("}\n");
  free(copy);
}

char *profile_for(const char *email) {
  char *copy = strdup(email);
  replace_chars(copy, '&', ' ');
  replace_chars(copy, '=', ' ');
  size_t cookie_size =
      strlen("email=") + strlen(copy) + strlen("&uid=10&role=user") + 1;
  char *cookie = (char *)malloc(cookie_size * sizeof(char));
  snprintf(cookie, cookie_size, "email=%s&uid=10&role=user", copy);
  free(copy);
  return cookie;
}

struct bytes encrypt(const char *profile) {
  size_t profile_len = strlen(profile);
  struct bytes padded = pkcs7(profile, profile_len, 16);
  struct bytes ciphertext =
      aes_encrypt_ecb((char *)padded.data, padded.len, aes_key, 16);
  free(padded.data);
  return ciphertext;
}

char *decrypt(const unsigned char *ciphertext, size_t ciphertext_len) {
  struct bytes decrypted =
      aes_decrypt_ecb(ciphertext, ciphertext_len, aes_key, 16);
  struct bytes stripped = pkcs7_strip(decrypted.data, decrypted.len);
  char *plaintext = (char *)malloc((stripped.len + 1) * sizeof(char));
  memcpy(plaintext, stripped.data, stripped.len);
  plaintext[stripped.len] = '\0';
  free(decrypted.data);
  free(stripped.data);
  return plaintext;
}

struct bytes byte_slice(const struct bytes *input, size_t start,
                        size_t length) {
  struct bytes res;
  res.data = (unsigned char *)malloc(length * sizeof(unsigned char));
  res.len = length;
  memcpy(res.data, input->data + start, length);
  return res;
}

struct bytes byte_concat(const struct bytes *a, const struct bytes *b) {
  struct bytes res;
  res.len = a->len + b->len;
  res.data = (unsigned char *)malloc(res.len * sizeof(unsigned char));
  memcpy(res.data, a->data, a->len);
  memcpy(res.data + a->len, b->data, b->len);
  return res;
}

int main(void) {
  srand(time(NULL));
  unsigned char *key = random_bytes(16);
  memcpy(aes_key, key, 16);
  free(key);
  char *email1 = "jack@pngu.org";
  char *profile1 = profile_for(email1);
  struct bytes encrypted1 = encrypt(profile1);
  struct bytes norole = byte_slice(&encrypted1, 0, 32);
  char *email2 = "0123456789admin\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
  char *profile2 = profile_for(email2);
  struct bytes encrypted2 = encrypt(profile2);
  struct bytes admin = byte_slice(&encrypted2, 16, 16);
  struct bytes payload = byte_concat(&norole, &admin);
  char *decrypted = decrypt(payload.data, payload.len);
  kv_pairs(decrypted);
  free(profile1);
  free(profile2);
  free(encrypted1.data);
  free(encrypted2.data);
  free(norole.data);
  free(admin.data);
  free(payload.data);
  free(decrypted);
  return 0;
}
