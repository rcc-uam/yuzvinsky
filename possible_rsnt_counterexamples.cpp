#include "common.h"
#include <algorithm>
#include <cmath>
#include <iostream>

bool valid_by_frequency_lemma(int r, int s, int n, int t) {
   return std::ceil(double(r * s) / n) <= t && t <= std::min(r, s);
}

bool valid_by_hidden_lemma(int r, int s, int n, int t) {
   return t == std::min(r, s) || n >= hopf_stiefel(t, r + s - t);
}

int main( ) {
   int r, s;
   while (std::cin >> r >> s) {
      int n = hopf_stiefel(r, s) - 1;
      for (int t = 1; t <= std::min(r, s); ++t) {
         if (valid_by_frequency_lemma(r, s, n, t) && valid_by_hidden_lemma(r, s, n, t)) {
            std::tie(r, s, t) = std::tuple_cat(label_order(r, s, t));
            std::cout << r << " " << s << " " << hopf_stiefel(r, s) - 1 << " " << t << "\n";
         }
      }
   }
}
