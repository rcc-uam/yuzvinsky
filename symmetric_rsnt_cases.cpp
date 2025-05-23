#include <algorithm>
#include <cmath>
#include <iostream>
#include "common.h"

int main( ) {
   int max_r;
   std::cin >> max_r;

   for (int r = 1; r <= max_r; ++r) {
      std::cout << r << " " << r << " " << max_colors(r, r, r) << " " << r << "\n";
   }
}
