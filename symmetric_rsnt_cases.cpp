#include <algorithm>
#include <cmath>
#include <iostream>
#include "common.h"

int main( ) {
   int size_limit;
   std::cin >> size_limit;

   for (int i = 1; i <= size_limit; ++i) {
      std::cout << i << " " << i << " " << max_colors(i, i, i) << " " << i << "\n";
   }
}
