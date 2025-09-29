#include "stats.h"

int frequency_score(const unsigned char *input, const size_t input_len) {
  static int freqs[] = {
      8167, 1492, 2782, 4253, 12702, 2228, 2015, 6094, 6966,
      153,  772,  4025, 2406, 6749,  7507, 1929, 95,   5987,
      6327, 9056, 2758, 978,  2360,  150,  1974, 74,
  };
  int score = 0;
  for (size_t i = 0; i < input_len; i++) {
    if (input[i] == 32)
      score += 10000;
    else if (input[i] >= 65 && input[i] <= 90)
      score += freqs[input[i] - 65];
    else if (input[i] >= 97 && input[i] <= 122)
      score += freqs[input[i] - 97];
    else if (input[i] >= 128)
      score -= 5000;
    else
      score -= 1000;
  }
  return score;
}

double index_coincidence(const char *input) {
  int n = 0, values[26] = {0}, curr;
  double sum = 0.0, total = 0.0;
  for (size_t i = 0; i < strlen(input); i++) {
    curr = input[i];
    if (curr >= 65 && curr <= 90) {
      values[curr - 65]++;
      n++;
    } else if (curr >= 97 && curr <= 122) {
      values[curr - 97]++;
      n++;
    }
  }
  for (size_t i = 0; i < 26; i++) {
    curr = values[i];
    sum += curr * (curr - 1);
  }
  total = sum / (n * (n - 1));
  return total;
}
