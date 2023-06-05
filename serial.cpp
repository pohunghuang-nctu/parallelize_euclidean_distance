// serial version of euclidean distance
#include <chrono>
#include <iostream>
#include <random>
#include <cmath>
#include "CycleTimer.h"

#define BATCH_SIZE 100000

float euclidean_distance_serial(const float* x, const float* y, const int d) {
    float distance = 0.0f;
    for (int i = 0; i < d; ++i) {
        float diff = x[i] - y[i];
        distance += diff * diff;
    }
    return distance;
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
    double startTime = CycleTimer::currentSeconds();
    for (int j = 0; j < BATCH_SIZE; j++) {
        z[j] = euclidean_distance_serial(x[j], y[j], d);
    }
    double endTime = CycleTimer::currentSeconds();
    printf("Serial version took: %.4f seconds.\n", endTime - startTime);
    double sum = 0.0;
    for (int i = 0; i < BATCH_SIZE; i++) {
        sum += z[i];
    }
    printf("sum: %.4f\n", sum);

    return 0;
}
