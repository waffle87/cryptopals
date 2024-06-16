// https://cryptopals.com/sets/3/challenges/19
// https://cryptopals.com/sets/3/challenges/20
#include "util/aes.h"
#include "util/base64.h"
#include "util/random.h"
#include "util/score.h"

/* [challenge 20]
 * data/c20_s3.txt contains base64'd plaintext. solve by using repeating-key
 * XOR'd algorithm. CTR encryption appears different from repeated-key XOR, but
 * with a fixed nonce, they are effectively the same thing. to make use of this
 * "exploit", take collection of ciphertexts and trucate them to a common
 * length. solve resulting concatenation of ciphertexts as if for repeating-key
 * XOR, with a key size of the legnth of ciphertext.
 */

unsigned char key[16];
char *strings[] = {"SSBoYXZlIG1ldCB0aGVtIGF0IGNsb3NlIG9mIGRheQ==",
                   "Q29taW5nIHdpdGggdml2aWQgZmFjZXM=",
                   "RnJvbSBjb3VudGVyIG9yIGRlc2sgYW1vbmcgZ3JleQ==",
                   "RWlnaHRlZW50aC1jZW50dXJ5IGhvdXNlcy4=",
                   "SSBoYXZlIHBhc3NlZCB3aXRoIGEgbm9kIG9mIHRoZSBoZWFk",
                   "T3IgcG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
                   "T3IgaGF2ZSBsaW5nZXJlZCBhd2hpbGUgYW5kIHNhaWQ=",
                   "UG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
                   "QW5kIHRob3VnaHQgYmVmb3JlIEkgaGFkIGRvbmU=",
                   "T2YgYSBtb2NraW5nIHRhbGUgb3IgYSBnaWJl",
                   "VG8gcGxlYXNlIGEgY29tcGFuaW9u",
                   "QXJvdW5kIHRoZSBmaXJlIGF0IHRoZSBjbHViLA==",
                   "QmVpbmcgY2VydGFpbiB0aGF0IHRoZXkgYW5kIEk=",
                   "QnV0IGxpdmVkIHdoZXJlIG1vdGxleSBpcyB3b3JuOg==",
                   "QWxsIGNoYW5nZWQsIGNoYW5nZWQgdXR0ZXJseTo=",
                   "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4=",
                   "VGhhdCB3b21hbidzIGRheXMgd2VyZSBzcGVudA==",
                   "SW4gaWdub3JhbnQgZ29vZCB3aWxsLA==",
                   "SGVyIG5pZ2h0cyBpbiBhcmd1bWVudA==",
                   "VW50aWwgaGVyIHZvaWNlIGdyZXcgc2hyaWxsLg==",
                   "V2hhdCB2b2ljZSBtb3JlIHN3ZWV0IHRoYW4gaGVycw==",
                   "V2hlbiB5b3VuZyBhbmQgYmVhdXRpZnVsLA==",
                   "U2hlIHJvZGUgdG8gaGFycmllcnM/",
                   "VGhpcyBtYW4gaGFkIGtlcHQgYSBzY2hvb2w=",
                   "QW5kIHJvZGUgb3VyIHdpbmdlZCBob3JzZS4=",
                   "VGhpcyBvdGhlciBoaXMgaGVscGVyIGFuZCBmcmllbmQ=",
                   "V2FzIGNvbWluZyBpbnRvIGhpcyBmb3JjZTs=",
                   "SGUgbWlnaHQgaGF2ZSB3b24gZmFtZSBpbiB0aGUgZW5kLA==",
                   "U28gc2Vuc2l0aXZlIGhpcyBuYXR1cmUgc2VlbWVkLA==",
                   "U28gZGFyaW5nIGFuZCBzd2VldCBoaXMgdGhvdWdodC4=",
                   "VGhpcyBvdGhlciBtYW4gSSBoYWQgZHJlYW1lZA==",
                   "QSBkcnVua2VuLCB2YWluLWdsb3Jpb3VzIGxvdXQu",
                   "SGUgaGFkIGRvbmUgbW9zdCBiaXR0ZXIgd3Jvbmc=",
                   "VG8gc29tZSB3aG8gYXJlIG5lYXIgbXkgaGVhcnQs",
                   "WWV0IEkgbnVtYmVyIGhpbSBpbiB0aGUgc29uZzs=",
                   "SGUsIHRvbywgaGFzIHJlc2lnbmVkIGhpcyBwYXJ0",
                   "SW4gdGhlIGNhc3VhbCBjb21lZHk7",
                   "SGUsIHRvbywgaGFzIGJlZW4gY2hhbmdlZCBpbiBoaXMgdHVybiw=",
                   "VHJhbnNmb3JtZWQgdXR0ZXJseTo=",
                   "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4="};

int main() {
  init_with_random_bytes(key, 16);
  size_t arr_len = sizeof(strings) / sizeof(strings[0]);
  unsigned char *outs[arr_len];
  size_t lens[arr_len], max_len = 0;
  // encrypt with random key
  for (uint i = 0; i < arr_len; i++) {
    size_t in_len;
    uint8_t *in = base64_decode(strings[i], &in_len);
    outs[i] = malloc(in_len);
    lens[i] = in_len;
    if (in_len > max_len)
      max_len = in_len;
    if (!ctr_encrypt(in, (int)in_len, key, 0, outs[i])) {
      fprintf(stderr, "ctr_encrypt failed\n");
      exit(1);
    }
  }
  uint8_t buf[arr_len], key_stream[max_len];
  for (uint j = 0; j < max_len; j++) {
    size_t cnt = 0;
    for (uint i = 0; i < arr_len; i++)
      if (j < lens[i])
        buf[cnt++] = outs[i][j];
    key_stream[j] = best_key_step(buf, cnt, 0, 1);
  }
  // print decrypted result (last characters
  // of lines are too difficult to break)
  for (uint i = 0; i < arr_len; i++)
    for (uint j = 0; j < max_len; j++) {
      if (j < lens[i])
        printf("%c\n", outs[i][j] ^ key_stream[j]);
      else
        break;
    }
  return 0;
}
