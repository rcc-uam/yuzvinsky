#include "common.h"
#include <algorithm>
#include <cmath>
#include <iostream>

int main( ) {
   int size_limit;
   std::cin >> size_limit;

   for (int r = 1; r <= size_limit; ++r) {
      for (int s = r; s <= size_limit; ++s) {
         for (int t = 1; t <= std::min(r, s); ++t) {
            std::cout << r << " " << s << " " << max_colors(r, s, t) << " " << t << "\n";
         }
      }
   }
}
