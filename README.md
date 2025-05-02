# A Computational Approach to Yuzvinsky's Conjecture on Intercalate Matrices

This repository provides a sample implementation of algorithms for computing intecalate matrices and a computational proof of Yuzvinsky's conjecture up to $32 \times 32$, as described in the paper "A Computational Approach to Yuzvinsky's Conjecture on Intercalate Matrices" by Francisco Zaragoza and Rodrigo Castro. The following source code files and folders are provided.

* `possible_rs_counterexamples.txt`: A list of integer pairs that denotes the set of possible $r \times s$ counterexamples of the Yuzvinsky's Conjecture up to $32 \times 32$.
* `possible_rsnt_counterexamples.cpp`: A C++ program that is intended to take the `possible_rs_counterexamples.txt` list as input, and produces a list of integer 4-tuples that denotes the set of possible $[r, s, n, t]$ counterexamples of the Yuzvinsky's Conjecture up to $32 \times 32$.
* `symmetric_rsnt_cases.cpp`: A C++ program that produces a list of integer 4-tuples that denotes the set of matrix types $[r, s, n, t]$ of all the non-isotopic symmetric intercalate matrices up to $16 \times 16$.
* `full_8_rsnt_cases`: A C++ program that produces a list of integer 4-tuples that denotes the set of matrix types $[r, s, n, t]$ of all the non-isotopic intercalate matrices up to $8 \times 8$.
* `intercalate_matrices.cpp`: A C++ program that asdasd. The program reads and writes files from the following folders:
  - `cache/`: asdasd
  - `cache_tmp/`: asdasd
  - `logs/`: asdasd
* `driver.cpp`: A C++ program that produces asdasd. It supports the following command-line argument:
  - `complete_recompute`: asdasd
* `print_matrices.cpp`: ASdasd. It supports the following command-line arguments:
  - `count_only`: asdasd
  - `n_as_bound`: asdasd
* `log_conjecture_proof.txt`: The output of the `print_matrices` program when taking as input the list computed by the `possible_rsnt_counterexamples` program, if done after running the driver program with the same such list.

Some additional logs from runs performed in our hardware are also provided. The C++ programs may be compiled with G++ 14.2 using the `-std=c++23` flag. However, the `intercalate_matrices` program is not intended to be compiled directly but from the driver program, as it needs additional flags that the latter provides. The algorithms were not optimized for microbenchmarking purposes, just for overall efficiency.
