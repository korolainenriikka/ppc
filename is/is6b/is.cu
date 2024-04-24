#include <iostream>
#include <vector>
#include <cstdlib>
#include <cuda_runtime.h>

struct Result {
    int y0;
    int x0;
    int y1;
    int x1;
    float outer[3];
    float inner[3];
};

static inline void check(cudaError_t err, const char* context) {
    if (err != cudaSuccess) {
        std::cerr << "CUDA error: " << context << ": "
            << cudaGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(x) check(x, #x)

static inline int divup(int a, int b) {
    return (a + b - 1)/b;
}

/**
 * x, y: upper left corner of rectangle
 * c: color component
 * size_x, size_y: size of rectangle
*/
int inner_sum(int x, int y, int size_x, int size_y, int nx, int* sums) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = (x+size_x-1) + nx * (y+size_y-1);
    int sum1 = sums[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    int sum2 = y > 0 ? sums[(x+size_x-1) + nx * (y-1)] : 0;
    
    // second blue square: x at x - 1, y at y + size_y
    int sum3 = x > 0 ? sums[(x-1) + nx * (y + size_y -1)] : 0;

    // last orange square
    int sum4 = (x > 0 && y > 0) ? sums[(x-1) + nx * (y-1)] : 0;

    int inner_sum = sum1 - sum2 - sum3 + sum4;
    return inner_sum;
}

__global__ void kernel(int* best_coords, float* best_sses, int* sums, int nx, int ny) {
    int size_x = threadIdx.x + blockIdx.x * blockDim.x;
    int size_y = threadIdx.y + blockIdx.y * blockDim.y;

    if (size_x > nx || size_y > ny || (size_x == nx && size_y == ny) || size_x == 0 || size_y == 0)
        return;

    int rec_size = size_y * size_x;
    int outer_size = nx*ny - rec_size;
    int total_sum = sums[nx*ny-1];

    float best_sse = total_sum;
    int best_x = 0;
    int best_y = 0;
    for (int y = 0; y <= ny - size_y; ++y) {
        for (int x = 0; x <= nx - size_x; ++x) {
            // inclusion/exclusion
            int sum1_i = (x+size_x-1) + nx * (y+size_y-1);
            int sum1 = sums[sum1_i];
            int sum2 = y > 0 ? sums[(x+size_x-1) + nx * (y-1)] : 0;
            int sum3 = x > 0 ? sums[(x-1) + nx * (y + size_y -1)] : 0;
            int sum4 = (x > 0 && y > 0) ? sums[(x-1) + nx * (y-1)] : 0;
            int in_sum = sum1 - sum2 - sum3 + sum4;

            int out_sum = total_sum - in_sum;

            // find inner and outer sse
            float sse_inner = in_sum * (1 - ((1.0/rec_size) * in_sum));
            float sse_outer = out_sum * (1 - ((1.0/outer_size) * out_sum));
            float sse = sse_inner + sse_outer;

            // compare sse to current minimum
            if (sse < best_sse) {
                best_sse = sse;
                best_y = y;
                best_x = x;
            }
        }
    }

    // write found solution to results
    // write y
    best_coords[0 + 2 * (size_x-1) + 2 * nx * (size_y-1)] = best_y;
    // write x
    best_coords[1 + 2 * (size_x-1) + 2 * nx * (size_y-1)] = best_x;
    // write sse
    best_sses[(size_x-1) + nx * (size_y-1)] = best_sse;
}


/*
This is the function you need to implement. Quick reference:
- x coordinates: 0 <= x < nx
- y coordinates: 0 <= y < ny
- color components: 0 <= c < 3
- input: data[c + 3 * x + 3 * nx * y]
*/
Result segment(int ny, int nx, const float *data) {
    // PREPROCESSING: create size nx*ny*3 array where each pixel is sum of that color component until that point
    std::vector<int> sums(nx*ny, 0);
    for (int y = 0; y < ny; ++y) {
        int row_sum = 0;
        for (int x = 0; x < nx; ++x) {
            int c = 0; // only read first color component
            int i = c + 3 * x + 3 * nx * y;
            int data_i = static_cast<int>(data[i]);
            row_sum += data_i;
            int sum_i_above = y == 0 ? 0 : sums[x + nx*(y-1)];
            sums[x+nx*y] = sum_i_above + row_sum;
        }
    }

    // Allocate GPU memory
    int* sumsGPU = NULL;
    CHECK(cudaMalloc((void**)&sumsGPU, nx*ny * sizeof(int)));
    CHECK(cudaMemcpy(sumsGPU, sums.data(), nx*ny * sizeof(int), cudaMemcpyHostToDevice));
    
    std::vector<int> init_coords(2*nx*ny, 1111);
    int* coordGPU = NULL;
    CHECK(cudaMalloc((void**)&coordGPU, 2*nx*ny * sizeof(int)));
    CHECK(cudaMemcpy(coordGPU, init_coords.data(), 2*nx*ny * sizeof(int), cudaMemcpyHostToDevice));

    float* sseGPU = NULL;
    CHECK(cudaMalloc((void**)&sseGPU, nx*ny * sizeof(float)));

    // Run kernel
    dim3 dimBlock(16, 16);
    dim3 dimGrid(divup(nx, dimBlock.x), divup(ny, dimBlock.y));
    kernel<<<dimGrid, dimBlock>>>(coordGPU, sseGPU, sumsGPU, nx, ny);
    CHECK(cudaGetLastError());

    // Copy data back to CPU
    std::vector<int> best_coords(2*nx*ny, 0);
    CHECK(cudaMemcpy(best_coords.data(), coordGPU, 2*nx*ny * sizeof(int) , cudaMemcpyDeviceToHost));
    
    std::vector<float> best_sses(nx*ny, 0.0);
    // -1: we do not write on the GPU to the last location as inner rec cannot be full rec
    CHECK(cudaMemcpy(best_sses.data(), sseGPU, (nx*ny-1)*sizeof(float), cudaMemcpyDeviceToHost));

    // Release memory
    CHECK(cudaFree(sumsGPU));
    CHECK(cudaFree(coordGPU));
    CHECK(cudaFree(sseGPU));

    // POSTPROCESSING

    // find best segmentation
    float min_sse = 600*600;
    // int best_size_x = 0;
    // int best_size_y = 0;
    Result best_result{0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};
    // benchmark 3.txt: y=ny, x=1 kohdassa ollaan muistissa paikassa johon ei olla kirjotettu.
    // siellä sse on pieni, ja systeemi luulee että jess hyvä segmentointi ja palauttaa sen.
    // jos laittaa size_y < ny, menee koordinaattien suhteen läpi. sillon inner/outer sum laskenta jostain syystä timeouttaa?
    // mut muut testit failaa, jos muuttaa ton <= pois (esim small simple 1)
    for (int size_y = 1; size_y <= ny; ++size_y) {
        for (int size_x = 1; size_x <= nx; ++size_x) {
            if (size_x == nx && size_y == ny) {
                continue;
            }
            
            float sse_for_size = best_sses[(size_x-1) + nx*(size_y-1)];
            if (sse_for_size < min_sse) {
                min_sse = sse_for_size;
                int y0 = best_coords[0 + 2 * (size_x-1) + 2 * nx * (size_y-1)];
                int x0 = best_coords[1 + 2 * (size_x-1) + 2 * nx * (size_y-1)];
                best_result.y0 = y0; 
                best_result.x0 = x0;
                best_result.y1 = y0 + size_y;
                best_result.x1 = x0 + size_x;
                // best_size_x = size_x;
                // best_size_y = size_y;
            }
        }
    }
    
    // find averages for best segmentation
    // int min_size_x = best_result.x1 - best_result.x0;
    // int min_size_y = best_result.y1 - best_result.y0;
    // int rec_size = min_size_x * min_size_y;
    // int outer_size = nx*ny - rec_size;
    // int in_sum = inner_sum(best_result.x0, best_result.y0, min_size_x, min_size_y, nx, sums.data());
    // float inner_avg = in_sum / rec_size;

    // int out_sum = sums[nx*ny-1] - in_sum;
    // float outer_avg = out_sum / outer_size;

    // for (int i = 10; i < 1000; ++i) {
    //     std::cout << best_sses[i] << ' ';
    // }
    // paras alkupiste on (291, 85). tosi monella koolla alusta, kaikilla koilla, toi on oikein.)
    // koko jonka löydän on (400,1) (y,x). ihan väärin.

    float inner_avg = 0.0;
    float outer_avg = 0.0;

    best_result.outer[0] = outer_avg;
    best_result.outer[1] = outer_avg;
    best_result.outer[2] = outer_avg;
    best_result.inner[0] = inner_avg;
    best_result.inner[1] = inner_avg;
    best_result.inner[2] = inner_avg;

    return best_result;
}