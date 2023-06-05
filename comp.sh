#!/bin/bash
g++ -O3 -fno-tree-vectorize -o serial serial.cpp
g++ -O3 -march=native -fno-tree-vectorize -mavx2 -o simd simd.cpp
nvcc -arch=sm_75 -O3 -o cuda cuda.cu