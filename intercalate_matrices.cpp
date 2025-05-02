#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS 1
#include "common.h"
#include <tbb/concurrent_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for_each.h>
#include <tbb/parallel_sort.h>
#include <algorithm>
#include <array>
#include <bitset>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <utility>

/*
#define R_VALUE 1
#define S_VALUE 1
#define N_VALUE 1
#define T_VALUE 1
*/

#if !defined(R_VALUE) || !defined(S_VALUE) || !defined(N_VALUE) || !defined(T_VALUE) || R_VALUE < 1 || S_VALUE < 1 || N_VALUE < 1 || T_VALUE < 1
   static_assert(false, "The following positive macro values are required: R_VALUE, S_VALUE, N_VALUE, T_VALUE");
#else
   constexpr int r = std::max(R_VALUE, S_VALUE), s = std::min(R_VALUE, S_VALUE), n = N_VALUE, t = T_VALUE;

   template<typename T, std::size_t E1, std::size_t E2>
   struct matrix : std::array<std::array<T, E2>, E1> { };

   template<typename T, std::size_t E1, std::size_t E2>
   struct augmented_matrix {
      const matrix<T, E1 - 1, E2>* submatrix;
      std::array<T, E2> extra_row;

      augmented_matrix( ) = default;
      augmented_matrix(const matrix<T, E1 - 1, E2>* sm, const matrix<T, E1, E2>& m)
      : submatrix(sm) {
         std::copy_n(&m[E1 - 1][0], E2, extra_row.data( ));
      }

      explicit operator matrix<T, E1, E2>( ) const {
         matrix<T, E1, E2> res;
         if (std::min(E1 - 1, E2) != 0) {
            std::copy_n(&(*submatrix)[0][0], (E1 - 1) * E2, &res[0][0]);
         }
         std::copy_n(extra_row.data( ), E2, &res[E1 - 1][0]);
         return res;
      }

      std::strong_ordering operator<=>(const augmented_matrix& m) const {
         auto res = (*submatrix <=> *m.submatrix);
         return (res != 0 ? res : (extra_row <=> m.extra_row));
      }
   };

   struct color_mapper {
      std::int8_t next_color;
      std::array<std::int8_t, n> table;

      color_mapper( )
      : next_color(0) {
         std::fill(table.begin( ), table.end( ), -1);
      }

      std::int8_t operator[](int current) {
         if (table[current] == -1) {
            table[current] = next_color++;
         }
         return table[current];
      }
   };

   template<std::size_t E1, std::size_t E2>
   int count_colors(const matrix<std::int8_t, E1, E2>& m) {
      return (std::min(E1, E2) != 0 ? *std::max_element(&m[0][0], &m[0][0] + E1 * E2) + 1 : 0);
   }

   template<std::size_t E1, std::size_t E2>
   matrix<std::int8_t, E2, E1> transpose(const matrix<std::int8_t, E1, E2>& m) {
      matrix<std::int8_t, E2, E1> res;
      for (int i = 0; i < E1; ++i) {
         for (int j = 0; j < E2; ++j) {
            res[j][i] = m[i][j];
         }
      }
      return res;
   }

   template<std::size_t E1, std::size_t E2>
   matrix<std::int8_t, E1, E2> color_reduction(const matrix<std::int8_t, E1, E2>& m) {
      matrix<std::int8_t, E1, E2> res;
      color_mapper color_map;
      for (int i = 0; i < E1; ++i) {
         for (int j = 0; j < E2; ++j) {
            res[i][j] = color_map[m[i][j]];
         }
      }
      return res;
   }

   template<std::size_t E1, std::size_t E2>
   std::array<int8_t, E1 * E2> layer_order_string(const matrix<int8_t, E1, E2>& m) {      // not clock-wise like described in the paper: from the edges of the layer to the center
      std::array<int8_t, E1 * E2> res;
      color_mapper color_map;
      for (int i = 0, w = 0; i < r; ++i) {
         for (int j = 0; j < std::min(i + 1, s); ++j) {
            res[w++] = color_map[m[i][j]];
            if (i != j && i < s) {
               res[w++] = color_map[m[j][i]];
            }
         }
      }
      return res;
   }

   struct metadata {
      std::array<std::array<std::int16_t, 2>, n> intercalation_count = { };
      std::array<std::vector<std::array<std::int8_t, r + s>>, n> relations;

      metadata(const matrix<std::int8_t, r, s>& m) {
         for (int i1 = 0; i1 < r - 1; ++i1) {
            for (int j1 = 0; j1 < s - 1; ++j1) {
               for (int i2 = i1 + 1; i2 < r; ++i2) {
                  for (int j2 = j1 + 1; j2 < s; ++j2) {
                     int arity_type = (m[i1][j1] == m[i2][j2]);
                     ++intercalation_count[m[i1][j1]][arity_type];
                     ++intercalation_count[m[i1][j2]][arity_type];
                     ++intercalation_count[m[i2][j1]][arity_type];
                     ++intercalation_count[m[i2][j2]][arity_type];
                  }
               }
            }
         }
         auto intercalation_count_by_color = intercalation_count;
         std::sort(intercalation_count.begin( ), intercalation_count.end( ));

         std::int8_t ranking[n], w = 0;
         for (int i = 0; i < n; ++i) {
            w += (i != 0 && intercalation_count[i - 1] < intercalation_count[i]);
            ranking[i] = w;
         }
         auto intercalation_ranking = [&](std::int8_t c) {
            return ranking[std::lower_bound(intercalation_count.begin( ), intercalation_count.end( ), intercalation_count_by_color[c]) - intercalation_count.begin( )];
         };

         for (int i = 0; i < r; ++i) {
            for (int j = 0; j < s; ++j) {
               std::array<std::int8_t, r + s> row_col_rel;
               for (int ti = 0; ti < r; ++ti) {
                  row_col_rel[ti] = intercalation_ranking(m[ti][j]);
               }
               for (int tj = 0; tj < s; ++tj) {
                  row_col_rel[r + tj] = intercalation_ranking(m[i][tj]);
               }
               std::sort(row_col_rel.begin( ), row_col_rel.end( ));
               relations[m[i][j]].push_back(row_col_rel);
            }
         }
         for (auto& current : relations) {
            std::sort(current.begin( ), current.end( ));
         }
         std::sort(relations.begin( ), relations.end( ));
      }

      std::strong_ordering operator<=>(const metadata&) const = default;
   };

   struct backtracking_state {
      int colors;
      matrix<std::int8_t, r, s> m;

      std::bitset<n> disallow_in_row[r];
      std::bitset<n> disallow_in_col[s];
      std::bitset<n> disallow_in_cell[s];
      std::int8_t color_position_in_column[s][n];

      backtracking_state(const matrix<std::int8_t, r - 1, s>& submatrix)
      : colors(count_colors(submatrix)) {
         if constexpr(r - 1 != 0) {
            std::copy_n(&submatrix[0][0], (r - 1) * s, &m[0][0]);
         }
         std::fill_n(&color_position_in_column[0][0], s * n, -1);

         std::int8_t frequency[n] = { };
         for (int i = 0; i < r - 1; ++i) {
            for (int j = 0; j < s; ++j) {
               std::int8_t c = m[i][j];
               frequency[c] += 1;
               disallow_in_row[i][c] = true;
               disallow_in_col[j][c] = true;
               color_position_in_column[j][c] = i;
            }
         }

         std::bitset<n> disallow_due_frequency;
         for (int c = 0; c < n; ++c) {
            disallow_due_frequency[c] = (frequency[c] == t);
         }

         for (int j = 0; j < s; ++j) {
            disallow_in_cell[j] = (
               disallow_in_col[j] |
               disallow_due_frequency |
               (r - 1 != 0 && j == 0 ? ~(~std::bitset<n>( ) << (m[r - 2][0] + 1)) : std::bitset<n>( )) |
               (r - 1 < t && r - 1 == j ? ~std::bitset<n>( ).set(0) : std::bitset<n>( ).set(0))
            );
         }
      }
   };

   template<int j>
   void compute_reduced_matrices(backtracking_state& state, auto&& callback) {
      if constexpr(j == s) {
         callback( );
      } else {
         std::bitset<n> allowed = ~(state.disallow_in_row[r - 1] | state.disallow_in_cell[j]);
         std::bitset<n> disallow_in_cell_backup[s - (j + 1)];
         if constexpr(s - (j + 1) != 0) {
            std::copy_n(&state.disallow_in_cell[j + 1], s - (j + 1), &disallow_in_cell_backup[0]);
         }

         for (int color = allowed._Find_first( ), previous_colors = state.colors; color < std::min(n, previous_colors + 1); color = allowed._Find_next(color)) {
            int still_possible = [&]( ) {
               for (int k = j + 1; k < s; ++k) {
                  std::int8_t h = state.color_position_in_column[k][color];
                  std::bitset<n> temp = state.disallow_in_cell[k] | (h != -1 ? ~std::bitset<n>( ).set(state.m[h][j]) : state.disallow_in_col[j]);
                  if (temp.all( )) {
                     return k;
                  }
                  state.disallow_in_cell[k] = temp;
               }
               return s;
            }( );
            if (still_possible == s) {
               state.m[r - 1][j] = color;
               state.colors += (color == previous_colors);
               state.disallow_in_row[r - 1][color] = true;
               compute_reduced_matrices<j + 1>(state, callback);
               state.disallow_in_row[r - 1][color] = false;
               state.colors -= (color == previous_colors);
            }
            if constexpr(s - (j + 1) != 0) {
               std::copy_n(&disallow_in_cell_backup[0], still_possible - (j + 1), &state.disallow_in_cell[j + 1]);
            }
         }
      }
   }

   struct trie {
      struct node {
         std::size_t leaves, index;
         std::bitset<n> in_node;
         std::unique_ptr<node[]> children;

         node* get_child(std::int8_t c) {
            return (in_node[c] ? &children[(in_node & ~(~std::bitset<n>( ) << c)).count( )] : nullptr);
         }
      } root;

      trie(const tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>& group) {
         std::vector<std::pair<std::array<int8_t, r * s>, std::size_t>> insert_data;
         for (std::size_t i = 0; i < group.size( ); ++i) {
            insert_data.emplace_back(layer_order_string(matrix<std::int8_t, r, s>(group[i])), i);
         }
         root = insert(insert_data.begin( ), insert_data.end( ), 0);
      }

      void remove(const augmented_matrix<std::int8_t, r, s>& m) {
         node* p = root_iterator( );
         p->leaves -= 1;
         for (std::int8_t c : layer_order_string(matrix<std::int8_t, r, s>(m))) {
            p = p->get_child(c);
            p->leaves -= 1;
         }
      }

      node* root_iterator( ) {
         return &root;
      }

      static std::size_t matrix_index(const node* p) {
         return p->index;
      }

      static bool descend(node*& p, std::int8_t c) {
         if (p != nullptr && p->leaves != 0) {
            p = p->get_child(c);
         }
         return p != nullptr && p->leaves != 0;
      }

   private:
      static node insert(auto begin, auto end, int tree_level) {
         node current;
         current.leaves = end - begin;
         if (tree_level == r * s) {
            current.index = begin->second;
         } else {
            std::sort(begin, end, [&](const std::pair<std::array<int8_t, r * s>, std::size_t>& a, const std::pair<std::array<int8_t, r * s>, std::size_t>& b) {
               return a.first[tree_level] < b.first[tree_level];
            });
            std::for_each(begin, end, [&](const std::pair<std::array<int8_t, r * s>, std::size_t>& a) {
               current.in_node[a.first[tree_level]] = true;
            });
            current.children = std::make_unique<node[]>(current.in_node.count( ));
            for (std::size_t i = 0; begin != end; ++i) {
               auto current_end = std::find_if(begin + 1, end, [&](const std::pair<std::array<int8_t, r * s>, std::size_t>& a) {
                  return a.first[tree_level] != begin->first[tree_level];
               });
               current.children[i] = insert(begin, current_end, tree_level + 1);
               begin = current_end;
            }
         }
         return current;
      }
   };

   template<int i>
   void permute(const matrix<std::int8_t, r, s>& m, color_mapper& color_map, std::array<int, r>& row_permutation, std::array<int, s>& col_permutation, trie::node* tree_iterator, auto&& callback) {
      if constexpr(i == r) {
         callback(trie::matrix_index(tree_iterator));
      } else {
         auto permute_next = [&](color_mapper color_map, trie::node* tree_iterator) {
            for (int j = 0; j < std::min(i + 1, s); ++j) {
               if (!trie::descend(tree_iterator, color_map[m[row_permutation[i]][col_permutation[j]]]) || (i != j && i < s && !trie::descend(tree_iterator, color_map[m[row_permutation[j]][col_permutation[i]]]))) {
                  return;
               }
            }
            permute<i + 1>(m, color_map, row_permutation, col_permutation, tree_iterator, callback);
         };

         for (int jr = i; jr < row_permutation.size( ); ++jr) {
            std::swap(row_permutation[i], row_permutation[jr]);
            if (i < col_permutation.size( )) {
               for (int jc = i; jc < col_permutation.size( ); ++jc) {
                  std::swap(col_permutation[i], col_permutation[jc]);
                  permute_next(color_map, tree_iterator);
                  std::swap(col_permutation[i], col_permutation[jc]);
               }
            } else {
               permute_next(color_map, tree_iterator);
            }
            std::swap(row_permutation[i], row_permutation[jr]);
         }
      }
   }

   void remove_isotopic(tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>& group) {
      std::sort(group.begin( ), group.end( ));
      trie tree(group);
      std::vector<bool> discard(group.size( ));

      std::size_t w = 0;
      for (std::size_t i = 0; i < group.size( ); ++i) {
         if (!discard[i]) {
            tree.remove(group[i]);
            color_mapper color_map;
            std::array<int, r> row_permutation; std::array<int, s> col_permutation;
            std::iota(row_permutation.begin( ), row_permutation.end( ), 0);
            std::iota(col_permutation.begin( ), col_permutation.end( ), 0);
            permute<0>(matrix<std::int8_t, r, s>(group[i]), color_map, row_permutation, col_permutation, tree.root_iterator( ), [&](std::size_t id) {
               if (!discard[id]) {
                  discard[id] = true;
                  tree.remove(group[id]);
               }
            });
            group[w++] = std::move(group[i]);
         }
      }
      group.resize(w);
   }

   int main( ) try {
      std::cout.setf(std::ios::unitbuf);
      std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
      std::cout.precision(2);

      const auto [rl, sl, tl] = label_order(r, s, t);
      std::array<bool, n + 1> computed_color_groups = { };
      #ifndef COMPLETE_RECOMPUTE
         for (int i = 1; i <= n; ++i) {
            computed_color_groups[i] = std::filesystem::exists(std::format("cache/{}_{}_{}_{}.dat", rl, sl, i, tl));
            if (computed_color_groups[i]) {
               std::cout << "Matrices with " << i << " colors already computed\n";
            }
         }
      #endif

      auto t0 = std::chrono::high_resolution_clock::now( );
      std::vector<matrix<std::int8_t, r - 1, s>> precomputed_submatrices;
      if (r - 1 == 0) {
         precomputed_submatrices.push_back(matrix<std::int8_t, r - 1, s>( ));
      } else {
         const auto [rpl, spl, tpl] = predecessor(r, s, t);
         for (int i = 1; i <= std::min(n, max_colors(rpl, spl, tpl)); ++i) {
            std::string input_path = std::format("cache/{}_{}_{}_{}.dat", rpl, spl, i, tpl);
            std::ifstream ifs(input_path, std::ios_base::binary);
            if (!ifs.is_open( )) {
               std::cout << "Could not open " << input_path << "\n";
               return 0;
            }
            if (r - 1 == rpl) {
               matrix<std::int8_t, r - 1, s> temp;
               while (ifs.read((char*)&temp[0][0], (r - 1) * s)) {
                  precomputed_submatrices.push_back(temp);
               }
            } else {
               matrix<std::int8_t, s, r - 1> temp;
               while (ifs.read((char*)&temp[0][0], s * (r - 1))) {
                  precomputed_submatrices.push_back(color_reduction(transpose(temp)));
               }
            }
         }
      }
      auto t1 = std::chrono::high_resolution_clock::now( );
      std::cout << precomputed_submatrices.size( ) << " precomputed submatrices loaded in " << std::chrono::duration<double>(t1 - t0).count( ) << " seconds (" << memory_get_peak_usage( ) / 1e6 << " MB of peak memory usage)\n";

      auto t2 = std::chrono::high_resolution_clock::now( );
      std::array<tbb::concurrent_map<metadata, tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>>, n + 1> found;
      tbb::parallel_for_each(precomputed_submatrices, [&](const matrix<std::int8_t, r - 1, s>& m) {
         backtracking_state state(m);
         compute_reduced_matrices<0>(state, [&] {
            if (!computed_color_groups[state.colors]) {
               found[state.colors][metadata(state.m)].emplace_back(&m, state.m);
            }
         });
      });
      auto t3 = std::chrono::high_resolution_clock::now( );
      std::cout << std::accumulate(found.begin( ), found.end( ), 0z, [](std::size_t count, const tbb::concurrent_map<metadata, tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>>& color_groups) {
         return count + std::accumulate(color_groups.begin( ), color_groups.end( ), 0z, [](std::size_t count, const auto& pair) {
            return count + pair.second.size( );
         });
      }) << " new reduced matrices found in " << std::chrono::duration<double>(t3 - t2).count( ) << " seconds (" << memory_get_peak_usage( ) / 1e6 << " MB of peak memory usage)\n";
      std::cout << std::accumulate(found.begin( ), found.end( ), 0z, [](std::size_t count, const tbb::concurrent_map<metadata, tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>>& color_groups) {
         return count + color_groups.size( );
      }) << " quasi-isotopy classes\n";

      auto t4 = std::chrono::high_resolution_clock::now( );
      std::array<tbb::concurrent_vector<augmented_matrix<std::int8_t, r, s>>, n + 1> per_color;
      for (int i = 1; i <= n; ++i) {
         tbb::parallel_for_each(found[i].begin( ), found[i].end( ), [&](auto& pair) {
            remove_isotopic(pair.second);
            std::copy(pair.second.begin( ), pair.second.end( ), per_color[i].grow_by(pair.second.size( )));
            pair.second.clear( ), pair.second.shrink_to_fit( );
         });
      }
      auto t5 = std::chrono::high_resolution_clock::now( );
      std::cout << std::accumulate(per_color.begin( ), per_color.end( ), 0z, [](std::size_t count, const auto& group) {
         return count + group.size( );
      }) << " new non-isotopic matrices found in " << std::chrono::duration<double>(t5 - t4).count( ) << " seconds (" << memory_get_peak_usage( ) / 1e6 << " MB of peak memory usage)\n";

      for (int i = 1; i <= n; ++i) {
         if (computed_color_groups[i]) {
            continue;
         }
         std::string output_path = std::format("cache_tmp/{}_{}_{}_{}.dat", rl, sl, i, tl);
         std::ofstream ofs(output_path, std::ios_base::binary);
         if (!ofs.is_open( )) {
            std::cout << "Could not open " << output_path << "\n";
            return 0;
         }
         tbb::parallel_sort(per_color[i].begin( ), per_color[i].end( ));
         for (const augmented_matrix<std::int8_t, r, s>& m : per_color[i]) {
            if (r == rl) {
               matrix<std::int8_t, r, s> temp(m);
               ofs.write((const char*)&temp[0][0], r * s);
            } else {
               matrix<std::int8_t, s, r> temp = color_reduction(transpose(matrix<std::int8_t, r, s>(m)));
               ofs.write((const char*)&temp[0][0], s * r);
            }
         }
         ofs.close( );
         std::rename(output_path.c_str( ), std::format("cache/{}_{}_{}_{}.dat", rl, sl, i, tl).c_str( ));
      }
   } catch (const std::exception& ex) {
      std::cout << ex.what( ) << "\n";
   }
#endif
