# Vector Distance Calculation Repository
This repository contains several implementations of vector distance calculation including a serial implementation, a SIMD (Single Instruction, Multiple Data) implementation, and a CUDA (Compute Unified Device Architecture) implementation.

## Files in this Repository
The following files are included in this repository:

    comp.sh - A script file for compiling the code.
    serial.cpp - C++ code for the serial implementation of vector distance calculation.
    simd.cpp - C++ code for the SIMD implementation of vector distance calculation.
    cuda.cu - CUDA code for the CUDA implementation of vector distance calculation.
    CycleTimer.h - A header file used in timing the code execution.

## Compilation
You can compile the code using the following commands:


```bash
g++ -O3 -fno-tree-vectorize -o serial serial.cpp
g++ -O3 -march=native -fno-tree-vectorize -mavx2 -o simd simd.cpp
nvcc -arch=sm_75 -O3 -o cuda cuda.cu
```
## Usage
The compiled programs take a single command line argument, which is the dimension of the vectors. The dimension must be a power of 2.

Here is an example of how to run the serial implementation with vectors of dimension 1024:

```bash
./serial 1024
```
