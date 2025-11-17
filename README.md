# A Computational Approach to Yuzvinsky's Conjecture on Intercalate Matrices

This repository provides a sample implementation of algorithms for computing intecalate matrices and a computational proof of Yuzvinsky's conjecture up to $32 \times 32$, as described in the paper "A Computational Approach to Yuzvinsky's Conjecture on Intercalate Matrices" by Rodrigo Castro, Alejandro González and Francisco Zaragoza. The following source code files and folders are provided.

* `possible_rsnt_counterexamples.cpp`: A C++ program that takes a list as integer pairs $r$ and $s$ as input, and produces a list of integer 4-tuples denoting the set of possible $[r, s, n, t]$ counterexamples of the Yuzvinsky's Conjecture.
* `symmetric_rsnt_cases.cpp`: A C++ program that produces a list of integer 4-tuples denoting the set of matrix types $[r, s, n, t]$ of all the non-isotopic symmetric intercalate matrices up to certain size.
* `full_rsnt_cases.cpp`: A C++ program that produces a list of integer 4-tuples denoting the set of matrix types $[r, s, n, t]$ of all the non-isotopic intercalate matrices up to certain size.
* `intercalate_matrices.cpp`: A C++ program that computes the non-isotopic intercalate matrices of type $[r, s, n, t]$. Compiled and called automatically by the driver program. The program reads and writes files from the following folders:
  - `cache/`: This folder stores the computed intercalate matrices of every successful run, including those from the preceding cases and from the current run. An $r$ `_` $s$ `_` $n$ `_` $t$ `.dat` file stores the non-isotopic matrices of type $[r, s, n, t]$ which have exactly $n$ colors. See https://callix.azc.uam.mx/rcc/yuzvinsky/ for a selected set of intercalate matrices available for download, and for instructions on how to interpret the contents of the generated files.
  - `cache_tmp/`: This folder stores files with partially computed results. Upon completion, the files are moved to the `cache/` folder. This is done to ensure that the `cache/` folder only contains fully computed results; this matters because the programs inspect which files are available in order to determine which cases are missing.
  - `logs/`: For each $[r, s, n, t]$ run of the `intercalate_matrices` program, this folder stores the corresponding log of the last executed run. Note that the value of $n$ identifies the program run and thus acts as an upper bound of the number of colors, while the value of $n$ of a `.dat` file in the `cache/` folder denotes an exact value. Hence, a single run of the `intercalate_matrices` program with a given $n$ generates a single log, but may generate multiple `.dat` files with $n' \leq n$.
* `driver.cpp`: A C++ program that takes a list of integer 4-tuples denoting the set of $[r, s, n, t]$ input types for which to compute their non-isotopic intercalate matrices, and that computes the dependency graph and launches the `intercalate_matrices` program accordingly. It supports the following command-line argument:
  - `complete_recompute`: Forces the recomputation of all the input cases and all their predecessor cases, even if they were already computed. This also forces the `intercalate_matrices` to recompute its whole result, even if some matrices with $n' \leq n$ colors were already computed.
* `print_matrices.cpp`: A C++ program that prints or counts the matrices of one or more $[r, s, n, t]$ types. A value of $-1$ for $n$ or for $t$ acts as a wildcard. The program supports the following command-line arguments:
  - `count_only`: Prints the number of matrices instead of printing their contents.
  - `n_as_bound`: If $n$ is not $-1$, considers every matrix with $n' \leq n$ colors. By default, only matrices with exactly $n$ colors are considered, as this matches the way in which the matrices are stored in the `cache/` folder.

Additionally, the `input_conjecture_32_rs.txt` file contains the list of integer pairs denoting the set of possible $r \times s$ counterexamples of the Yuzvinsky's Conjecture up to $32 \times 32$. The output of the `possible_rsnt_counterexamples` program taking the previous file as input, together with outputs of the `symmetric_rsnt_cases` and `full_rsnt_cases` programs, are also provided in the files `input_conjecture_32_rsnt.txt`, `input_symmetric_16_rsnt.txt` and `input_full_8_rsnt.txt`, respectively.

We also include some logs from runs performed in our hardware. Finally, the `log_conjecture_32_proof.txt` is the output of the `print_matrices` program using the `count_only` and `n_as_bound` arguments, when taking as input the `input_conjecture_32_rsnt.txt` file. This log must be produced after running the driver program with the same input file.

The C++ programs depend on the Intel® oneAPI Threading Building Blocks and may be compiled with G++ 13.3.0 using the `-std=c++23` flag. However, the `intercalate_matrices` program is not intended to be compiled directly but from the driver program, as it needs additional flags that the latter provides. The algorithms were not optimized for microbenchmarking purposes, just for overall efficiency. On Ubuntu 24.04, the following commands may be used to compile and run the programs:

```
sudo apt-get update
sudo apt install wget unzip g++ libtbb-dev

wget https://github.com/rcc-uam/yuzvinsky/archive/refs/heads/main.zip -O yuzvinsky-main.zip
unzip yuzvinsky-main.zip
cd yuzvinsky-main

g++ -std=c++23 driver.cpp -o driver
g++ -std=c++23 full_rsnt_cases.cpp -o full_rsnt_cases
g++ -std=c++23 possible_rsnt_counterexamples.cpp -o possible_rsnt_counterexamples
g++ -std=c++23 symmetric_rsnt_cases.cpp -o symmetric_rsnt_cases
g++ -std=c++23 print_matrices.cpp -o print_matrices
./driver < input_conjecture_32_rsnt.txt                                 # compute the data to prove the Yuzvinsky conjecture
./print_matrices count_only n_as_bound < input_conjecture_32_rsnt.txt   # verify the conjecture
```
