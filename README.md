# Textile Cutting Optimization Tool

This repository contains three C++ implementations of the **Textile Cutting Optimization** problem for planning and processing production in a textile factory. Given a single loom width and a set of rectangular orders, each program computes how to cut a minimal-length roll to satisfy all orders under different algorithmic strategies.

## Table of Contents

- [Problem Statement](#problem-statement)
- [Algorithms](#algorithms)
- [Repository Structure](#repository-structure)
- [Build & Usage](#build--usage)
- [Performance & Limits](#performance--limits)
- [Authors](#authors)
- [License](#license)

---

## Problem Statement

A textile factory weaves fabric rolls of fixed width `W` and unbounded length `L`, then cuts out ordered rectangles of various sizes.

**Goal:** Compute the **minimum roll length** `L` and a valid packing of all orders into that roll, such that no rectangles overlap and every order’s dimensions appear.

- Orders are given as counts of rectangles of size `pᵢ × qᵢ`.
- Rectangles may be rotated.
- Coordinate system: origin `(0,0)` at top-left; `x` grows right to `W−1`, `y` grows down to `L−1`.

---

## Algorithms

Three strategies are provided:

1. **Exhaustive Search (`exh.cc`)**
   - Explores all cutting patterns up to a time limit.
   - On finding an improved solution, overwrites the output so the partial best is always saved.

2. **Greedy Heuristic (`greedy.cc`)**
   - Fast, rule-based placement.
   - Expected to produce a valid but non-optimal roll length instantly, even on large test cases.

3. **Metaheuristic (`mh.cc`)**
   - Implements a class-taught metaheuristic (e.g., simulated annealing or genetic approach).
   - Improves solutions over time; writes intermediate best when found.

---

## Repository Structure

```text
├── exh.cc           # Exhaustive search implementation
├── greedy.cc        # Greedy heuristic implementation
├── mh.cc            # Metaheuristic implementation
├── checker          # Provided checker tool (compiled separately)
├── 10-30.inp        # Medium Input file
├── 11_13.inp        # Small Input file
├── 9-99.inp         # Large Input file
├── README.md
└── LICENSE
```
---

## Build & Usage

1. **Compile** each C++ file (requires C++17):

 ```bash
 g++ -std=c++17 -O2 exh.cc -o exh
 g++ -std=c++17 -O2 greedy.cc -o greedy
 g++ -std=c++17 -O2 mh.cc -o mh
```

2. **Run** with an input file and desired output file:

   ```bash
   ./exh data/sample.in sample_exh.out
   ./greedy data/sample.in sample_greedy.out
   ./mh data/sample.in sample_mh.out
   ```


3. Check correctness using the provided checker:
./checker data/sample.in sample_mh.out

---

## Performance & Limits
- Exhaustive: time limit ≤ 3 minutes.

- Greedy: must finish ≤ 5 seconds on largest inputs.

- Metaheuristic: time limit ≤ 1 minute.

Ensure all executables respect these limits when evaluated on test instances of similar size to the provided dataset.

## Authors
- **Lucas Federico Viner**

Algorithms & Programming III, Universitat Politècnica de Catalunya (UPC), January 2024.

## License

This project is for academic purposes.


