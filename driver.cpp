#include "common.h"
#include <algorithm>
#include <compare>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

constexpr std::string platform_prefix =
 #ifdef _WIN32
   ""
 #else
   "./"
 #endif
;

constexpr std::string platform_postfix =
 #ifdef _WIN32
   ".exe"
 #else
   ""
 #endif
;

void compute_pending(int r, int s, int n, int t, std::map<std::array<int, 3>, int>& max_n, std::set<std::array<int, 3>>& listed, std::vector<std::array<int, 3>>& listing_order) {
   if (s > 1) {
      auto [rp, sp, tp] = predecessor(r, s, t);
      compute_pending(rp, sp, n, tp, max_n, listed, listing_order);
   }
   max_n[{ r, s, t }] = std::max(max_n[{ r, s, t }], std::min(max_colors(r, s, t), n));
   if (listed.insert({ r, s, t }).second) {
      listing_order.push_back({ r, s, t });
   }
}

int main(int argc, const char* argv[]) {
   std::cout.setf(std::ios::unitbuf);

   std::map<std::array<int, 3>, int> max_n;
   std::set<std::array<int, 3>> listed;
   std::vector<std::array<int, 3>> listing_order;

   int r, s, n, t;
   while (std::cin >> r >> s >> n >> t) {
      if (n < std::max(r, s)) {
         std::cout << "Skipping " << r << " " << s << " " << n << " " << t << " (too few colors)\n";
         continue;
      }
      std::tie(r, s, t) = std::tuple_cat(label_order(r, s, t));
      compute_pending(r, s, n, t, max_n, listed, listing_order);
   }

   std::string executable_path = platform_prefix + "intercalate_matrices" + platform_postfix;
   bool complete_recompute = (argc > 1 && std::strcmp(argv[1], "complete_recompute") == 0);
   for (auto [r, s, t] : listing_order) {
      std::cout << r << " " << s << " " << max_n[{ r, s, t }] << " " << t << "\n";
      if (!complete_recompute && std::filesystem::exists(std::format("cache/{}_{}_{}_{}.dat", r, s, max_n[{ r, s, t }], t))) {
         std::cout << "already computed\n";
      } else {
         std::cout << "   compiling...\n";
         std::system(std::format("g++ -std=c++23 -D R_VALUE={} -D S_VALUE={} -D N_VALUE={} -D T_VALUE={} {} intercalate_matrices.cpp -O3 -march=native -ltbb -lpthread -o {}",
            r, s, max_n[{ r, s, t }], t, (complete_recompute ? "-D COMPLETE_RECOMPUTE" : ""), executable_path
         ).c_str( ));
         std::cout << "   running...\n";
         std::system(std::format("{} > logs/{}_{}_{}_{}.txt", executable_path, r, s, max_n[{ r, s, t }], t).c_str( ));
         std::remove(executable_path.c_str( ));
      }
   }
}
