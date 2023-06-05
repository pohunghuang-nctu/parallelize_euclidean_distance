#include <cuda_runtime.h>
#include <cuda.h>
#include <stdio.h>
#include "CycleTimer.h"
#include <chrono>
#include <iostream>
#include <random>
#include <cmath>


#define BATCH_SIZE 100000
#define THREADS_PER_BLOCK 256

// CUDA Kernel for euclidean distance
__device__ float square_diff(float x, float y) {
    return (x - y) * (x - y);
}

__global__ void euclidean_distance_cuda(float* z, float *x, float *y, int d) {
    int index = blockIdx.y + THREADS_PER_BLOCK * threadIdx.x;
    float sqr_diff = 0.0;
    if (index < d) {
        sqr_diff = square_diff(x[index], y[index]);
        atomicAdd(z, sqr_diff);
    }
}

// Host function for euclidean distance using CUDA
void euclidean_distance(float* z, float *x, float *y, int d) {
    dim3 blocksPerGrid(1, d / THREADS_PER_BLOCK);
    dim3 threadsPerBlock(THREADS_PER_BLOCK, 1);
    euclidean_distance_cuda<<<blocksPerGrid, threadsPerBlock>>>(z, x, y, d);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [dimension]\n", argv[0]);
        exit(-1);
    }

    int d = atoi(argv[1]);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0, 1.0);

    float** x = (float**)malloc(BATCH_SIZE * sizeof(float*));
    float** y = (float**)malloc(BATCH_SIZE * sizeof(float*));
    float* z = (float*)malloc(BATCH_SIZE * sizeof(float));
    for(int i = 0; i < BATCH_SIZE; i++) {
        x[i] = (float*)malloc(d * sizeof(float));
        y[i] = (float*)malloc(d * sizeof(float));
    }

    for (int j = 0; j < BATCH_SIZE; j++) {
        for (int i = 0; i < d; ++i) {
            x[j][i] = dis(gen);
            y[j][i] = dis(gen);
        }
    }
    float *z_dev;
    float *x_dev;
    float *y_dev;
    double startTime = CycleTimer::currentSeconds();
    cudaMalloc((void**)&z_dev, BATCH_SIZE * sizeof(float));
    cudaMalloc((void**)&x_dev, d * sizeof(float));
    cudaMalloc((void**)&y_dev, d * sizeof(float));
    double accTime = 0.0;
    double s_in = 0.0;
    double e_in = 0.0;
    for (int i = 0; i < BATCH_SIZE; i++) {
        cudaMemcpy(x_dev, x[i], d * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(y_dev, y[i], d * sizeof(float), cudaMemcpyHostToDevice);
        s_in = CycleTimer::currentSeconds();
        euclidean_distance(&z_dev[i], x_dev, y_dev, d);
        e_in = CycleTimer::currentSeconds();
        accTime += (e_in - s_in);
        //printf("z: %.4f\n", z[i]);
    }
    cudaDeviceSynchronize();
    double endTime = CycleTimer::currentSeconds();
    cudaMemcpy(z, z_dev, BATCH_SIZE * sizeof(float), cudaMemcpyDeviceToHost);
    printf("CUDA version took: %.4f seconds.\n", endTime - startTime);
    printf("CUDA kernels took: %.4f seconds for %d iterations.\n", accTime, BATCH_SIZE);
    double sum = 0.0;
    for (int i = 0; i < BATCH_SIZE; i++) {
        sum += z[i];
    }
    printf("sum: %.4f\n", sum);

    return 0;
}
