#include "common.h"
#include <algorithm>
#include <cmath>
#include <iostream>

int main( ) {
   int max_r, max_s;
   std::cin >> max_r >> max_s;

   for (int r = 1; r <= max_r; ++r) {
      for (int s = r; s <= max_s; ++s) {
         for (int t = 1; t <= std::min(r, s); ++t) {
            std::cout << r << " " << s << " " << max_colors(r, s, t) << " " << t << "\n";
         }
      }
   }
}
