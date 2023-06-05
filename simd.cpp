#include <cstdio>
#include <immintrin.h>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <cmath>
#include "CycleTimer.h"

#define BATCH_SIZE 100000

float euclidean_distance_simd(const float* x, const float* y, const int d) {
    __m256 sum = _mm256_setzero_ps();
    for (int i = 0; i < d; i += 8) {
        //printf("I am here. fuck you at %d\n", i);
        //fflush(stdout);    
        //printf("x[i] = %.2f", x[i]);
        //fflush(stdout); 
        __m256 a = _mm256_loadu_ps(x + i);
        //printf("I am _mm256_load_ps x. fuck you at %d\n", i);
        //fflush(stdout);
        __m256 b = _mm256_loadu_ps(&y[i]);
        //printf("I am _mm256_load_ps y. fuck you at %d\n", i);
        //fflush(stdout);
        __m256 diff = _mm256_sub_ps(a, b);
        //printf("I am _mm256_sub_ps. fuck you at %d\n", i);
        //fflush(stdout);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));
        //printf("I am _mm256_mul_ps. fuck you at %d\n", i);
        //fflush(stdout);
    }
    __m256 hsum = _mm256_hadd_ps(sum, sum);
    float result[8];
    _mm256_storeu_ps(result, hsum);
    return result[0] + result[2] + result[4] + result[6];
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [dimension]\n";
        return -1;
    }

    int d = std::stoi(argv[1]);
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

    //std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    // float distance_serial = euclidean_distance_serial(x, y, d);
    // std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    //printf("die here??\n");
    //fflush(stdout);

    double startTime = CycleTimer::currentSeconds();
    for (int i = 0; i < BATCH_SIZE; i++) {
        z[i] = euclidean_distance_simd(x[i], y[i], d);
        //printf("die here??\n");
        //fflush(stdout);
    }
    //printf("die here??\n");
    //fflush(stdout);
    double endTime = CycleTimer::currentSeconds();
    //std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

    // float distance_cuda = euclidean_distance_cuda(x, y, d);
    // std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();

    // auto duration_serial = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
    //auto duration_simd = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t1).count() * BATCH_SIZE00;
    // auto duration_cuda = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

    // std::cout << "Serial version took: " << duration_serial << " micro seconds.\n";
    // std::cout << "SIMD version took: " << duration_simd << " seconds.\n";
    // std::cout << "CUDA version took: " << duration_cuda << " microseconds.\n";
    printf("SIMD version took: %.4f seconds.\n", endTime - startTime);
    double sum = 0.0;
    for (int i = 0; i < BATCH_SIZE; i++) {
        sum += z[i];
    }
    printf("sum: %.4f\n", sum);

    return 0;
}
