#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <tuple>
#include <bit>
#ifdef _WIN32
   #include <windows.h>
   #include <psapi.h>
#else
   #include <unistd.h>
   #include <sys/resource.h>
#endif

int max_colors(int r, int s, int t) {
   return r * s - t * t + (t * t - t) / 2 + 1;
}

std::array<int, 3> label_order(int r, int s, int t) {
   return { std::min(r, s), std::max(r, s), std::min({ r, s, t }) };
}

std::array<int, 3> predecessor(int r, int s, int t) {
   std::tie(r, s, t) = std::tuple_cat(label_order(r, s, t));
   if (r == s) {
      return { r - 1, s, std::min(r - 1, t) };
   } else {
      return { r, s - 1, t };
   }
}

constexpr int hopf_stiefel(int r, int s) {
   if (r > s) {
      return hopf_stiefel(s, r);
   } else if (r == 1) {
      return s;
   } else if (auto sbound = std::bit_ceil(unsigned(s)); sbound / 2 < r && r <= s && s <= sbound) {
      return sbound;
   } else if (auto rbound = std::bit_ceil(unsigned(r)); rbound / 2 < r && r <= rbound && rbound < s) {
      return rbound + hopf_stiefel(r, s - rbound);
   } else {
      return 0;
   }
}

std::size_t memory_get_peak_usage( ) {
 #ifdef WIN32
   PROCESS_MEMORY_COUNTERS aux;
   GetProcessMemoryInfo(GetCurrentProcess( ), &aux, sizeof(aux));
   return aux.PeakWorkingSetSize;
 #else
   rusage aux;
   getrusage(RUSAGE_SELF, &aux);
   #ifdef __APPLE__
      return aux.ru_maxrss;
   #else
      return aux.ru_maxrss * 1024;
   #endif
 #endif
}

#endif
