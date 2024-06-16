// https://cryptopals.com/sets/2/challenges/13
#include "util/aes.h"
#include "util/random.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_KEY_LEN 16
#define MAX_VAL_LEN 128

enum state { START, KEY, EQ, VAL };
static unsigned char c_key[16];
struct profile {
  int uid;
  char *email;
  char *role;
};

static char *serialize_profile(struct profile *p) {
  char *str = NULL;
  asprintf(&str, "email=%s&uid=%d&role=%s", p->email, p->uid,
           p->role); // asprintf for memory safety :^)
  return str;
}

// far too laborous parser...
static struct profile *parse_profile(char *str) {
  struct profile *profile = malloc(sizeof(struct profile));
  char *c = str;
  enum state s = START;
  char key[MAX_KEY_LEN] = {0}, val[MAX_VAL_LEN] = {0};
  size_t key_ix = 0, val_ix = 0;
  while (*c != '\0') {
    switch (s) {
    case START:
      // expect start of key
      if (isalpha(*c) || *c == '_') {
        key[key_ix++] = *c;
        s = KEY;
      } else {
        printf("expected start of key, but got: %c\n", *c);
        exit(1);
      }
      break;
    case KEY:
      if (isalnum(*c) || *c == '_') {
        if (key_ix == MAX_KEY_LEN) {
          printf("max key len exceeded: %d chars\n", MAX_KEY_LEN);
          exit(1);
        }
        // continue with current key
        key[key_ix++] = *c;
      } else if (*c == '=') {
        // NULL-terminate key
        key[key_ix] = '\0';
        s = EQ;
      } else {
        printf("expected continuation of key or '=', but got: %c\n", *c);
        exit(1);
      }
      break;
    case EQ:
      /*
       * we could be smarter here and prevent non-printable characters,
       * but actually the task itself has a solution which requires using
       * '\x0b' char in the value. so we can simply skip '=' and '&' to disable
       * SQL-injection style attacks.
       */
      if (*c != '=' && *c != '&') {
        val[val_ix++] = *c;
        s = VAL;
      }
      break;
    case VAL:
      if (val_ix == MAX_VAL_LEN) {
        printf("max val len exceeded: %d chars\n", MAX_VAL_LEN);
        exit(1);
      }
      if (*c == '&') {
        val[val_ix] = '\0';
        s = START;
        key_ix = 0;
        val_ix = 0;
        if (strcmp(key, "email") == 0)
          profile->email = strdup(val);
        else if (strcmp(key, "uid") == 0)
          profile->uid = atoi(val);
        else if (strcmp(key, "role") == 0)
          profile->role = strdup(val);
        else {
          printf("unknown key: %s\n", key);
          exit(1);
        }
      } else if (*c != '=') {
        val[val_ix++] = *c;
        s = VAL;
      }
      break;
    }
    c++;
  }
  if (s == VAL) {
    val[val_ix] = '\0';
    if (strcmp(key, "email") == 0)
      profile->email = strdup(val);
    else if (strcmp(key, "uid") == 0)
      profile->uid = atoi(val);
    else if (strcmp(key, "role") == 0)
      profile->role = strdup(val);
    else {
      printf("unknown key: %s\n", key);
      exit(1);
    }
  }
  return profile;
}

static char *profile_for(char *email) {
  if (strchr(email, '=') != NULL || strchr(email, '&') != NULL) {
    printf("no & and = in emails pls\n");
    exit(1);
  }
  struct profile *p = malloc(sizeof(struct profile));
  p->uid = 10;
  p->email = email;
  p->role = "user";
  char *res = serialize_profile(p);
  free(p);
  return res;
}

static int encrypt_profile(char *profile_str, unsigned char *out) {
  EVP_CIPHER_CTX *ctx = evp_init();
  int encrypted_len = evp_ecb_encrypt(ctx, (unsigned char *)profile_str,
                                      strlen(profile_str), c_key, out);
  evp_cleanup(ctx);
  return encrypted_len;
}

static int decrypt_profile(unsigned char *in, int in_len, unsigned char *out) {
  EVP_CIPHER_CTX *ctx = evp_init();
  int decrypted_len = evp_ecb_decrypt(ctx, in, in_len, c_key, out);
  evp_cleanup(ctx);
  return decrypted_len;
}

static struct profile *make_admin_profile() {
  /*
   * first create a profile for specially crafted username (10 bytes + "admin"
   * + padding to full block) to have encrypted version of "admin" end of the
   * string: 0123456789abcdef | 0123456789abcdef | 0123456789abcdef |
   * 0123456789abcdef email=aaaaaaaaaa | admin........... | &uid=10&role=use |
   * r.............. cut this block ^
   */
  char *fake = "aaaaaaaaaa"
               "admin"
               "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
  // 11 chars to pad, so we use '0x0b * 11'
  unsigned char encrypted_fake[64];
  encrypt_profile(profile_for(fake), encrypted_fake);
  /*
   * now replace 'user' part with block we cut before:
   * 0123456789abcdef | 0123456789abcdef | 0123456789abcdef | 0123456789abcdef
   * email=jack@test. | xyz&uid=10&role= | user             |
   *                                       paste block here ^
   */
  unsigned char encrypted[48];
  encrypt_profile(profile_for("jack@test.xyz"), encrypted);
  // THE cut and paste
  memcpy(&encrypted[32], &encrypted_fake[16], 16);
  // decrypt, NULL-terminate, & parse
  unsigned char decrypted[48];
  int decrypted_len = decrypt_profile(encrypted, 48, decrypted);
  decrypted[decrypted_len] = '\0';
  return parse_profile((char *)decrypted);
}

int main() {
  init_with_random_bytes(c_key, 16);
  struct profile *admin = make_admin_profile();
  printf("%s\n", serialize_profile(admin));
  return 0;
}
