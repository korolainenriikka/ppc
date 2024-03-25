#include <cmath>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cuda_runtime.h>

static inline int divup(int a, int b) {
    return (a + b - 1)/b;
}

static inline void check(cudaError_t err, const char* context) {
    if (err != cudaSuccess) {
        std::cerr << "CUDA error: " << context << ": "
            << cudaGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(x) check(x, #x)

__global__ void kernel(float* result, float* normalized_T, int ny, int nx) {
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;

    if (x >= ny || y >= ny) {
        return;
    }

    if (x < y) {
        result[x+y*ny] = 0;
        return;
    }

    double dot_product = 0.0;
    for (int k = 0; k < nx; ++k) {
        dot_product += normalized_T[k*ny + y] * normalized_T[k*ny + x];
    }
    result[x + y * ny] = dot_product;
}

void correlate(int ny, int nx, const float *data, float *result) {
    // NORMALIZATION
    std::vector<float> normalized(ny*nx, 0.0);

    // vähennä mean jokaisesta elementistä
    for (int y = 0; y < ny; ++y) {
        float sum = 0;
        for (int x = 0; x < nx; ++x) {
            sum += data[x + y*nx];
        }
        float mean = sum/nx;
    
        for (int x = 0; x < nx; ++x) {
            if (mean != 0) {
                normalized[x + y*nx] = data[x + y*nx] - mean;
            } else {
                normalized[x + y*nx] = data[x + y*nx];
            }
        }
    }

    // normalize the input rows so that for each row the sum of the squares of the elements is 1
    for (int y = 0; y < ny; ++y) {
        float sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            sum_of_squares += normalized[x + y*nx]*normalized[x + y*nx];
        }
        float sqrt_sum_of_squares = sqrt(sum_of_squares);

        for (int x = 0; x < nx; ++x) {
            normalized[x + y*nx] = normalized[x + y*nx] / sqrt_sum_of_squares;
        }
    }
    std::vector<float> normalized_T(ny*nx, 0.0);
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            normalized_T[y + x*ny] = normalized[x + y*nx];
        }
    }

    // Copy data GPU
    float* dGPU = NULL;
    CHECK(cudaMalloc((void**)&dGPU, nx * ny * sizeof(float)));
    float* rGPU = NULL;
    CHECK(cudaMalloc((void**)&rGPU, ny * ny * sizeof(float))); // result size is ny x ny
    // CHECK(cudaMemset((void**)&rGPU, 0, ny * ny * sizeof(float)));
    CHECK(cudaMemcpy(dGPU, normalized_T.data(), nx * ny * sizeof(float), cudaMemcpyHostToDevice));

    // Run kernel
    dim3 dimBlock(16, 16);
    dim3 dimGrid(divup(ny, dimBlock.x), divup(ny, dimBlock.y));
    kernel<<<dimGrid, dimBlock>>>(rGPU, dGPU, ny, nx);
    CHECK(cudaGetLastError());

    // Copy data back to CPU & release memory
    CHECK(cudaMemcpy(result, rGPU, ny * ny * sizeof(float), cudaMemcpyDeviceToHost));
    CHECK(cudaFree(dGPU));
    CHECK(cudaFree(rGPU));
}