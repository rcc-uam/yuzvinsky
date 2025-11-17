#include "common.h"
#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <tuple>
#include <vector>

int main(int argc, const char* argv[]) {
   bool count_only = std::any_of(&argv[0] + 1, &argv[0] + argc, [](const char* p) {
      return std::strcmp(p, "count_only") == 0;
   });
   bool n_as_bound = std::any_of(&argv[0] + 1, &argv[0] + argc, [](const char* p) {
      return std::strcmp(p, "n_as_bound") == 0;
   });

   auto print = [&](int r, int s, int n, int t, std::size_t& total) {
      std::string input_file = std::format("cache/{}_{}_{}_{}.dat", r, s, n, t);
      std::cout << input_file << ": ";
      std::ifstream ifs(input_file, std::ios_base::binary);
      if (!ifs.is_open( )) {
         std::cout << "could not open file\n";
         return;
      }
      std::size_t count = std::filesystem::file_size(input_file) / (r * s);
      total += count;
      std::cout << count << " matrices\n";

      if (!count_only) {
         std::vector<std::int8_t> temp(r * s);
         while (ifs.read((char*)temp.data( ), r * s)) {
            for (int i = 0; i < r; ++i) {
               for (int j = 0; j < s; ++j) {
                  std::cout << (int)temp[i * s + j] << " ";
               }
               std::cout << "\n";
            }
            std::cout << "\n";
         }
      }
   };

   int r, s, n, t;
   while (std::cin >> r >> s >> n >> t) {
      std::size_t total = 0;
      std::tie(r, s, t) = std::tuple_cat(label_order(r, s, t));
      std::cout << r << " " << s << " " << n << " " << t << "...\n";
      auto [min_t, max_t] = (t == -1 ? std::pair(1, std::min(r, s)) : std::pair(t, t));
      for (int ti = min_t; ti <= max_t; ++ti) {
         auto [min_n, max_n] = (n == -1 ? std::pair(1, max_colors(r, s, ti)) : (n_as_bound ? std::pair(1, std::min(n, max_colors(r, s, ti))) : std::pair(n, n)));
         for (int ni = min_n; ni <= max_n; ++ni) {
            print(r, s, ni, ti, total);
         }
      }
      std::cout << total << " matrices in total\n";
   }
}
