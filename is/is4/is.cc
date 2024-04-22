#include <iostream>
#include <vector>
#include <omp.h>

typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));

const double4_t zeros = {0.0, 0.0, 0.0, 0.0}; 

struct Result {
    int y0;
    int x0;
    int y1;
    int x1;
    float outer[3];
    float inner[3];
};

/**
 * x, y: upper left corner of rectangle
 * c: color component
 * size_x, size_y: size of rectangle
*/
double4_t inner_sum(int x, int y, int size_x, int size_y, int nx, std::vector<double4_t>& sums) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = (x+size_x-1) + nx * (y+size_y-1);
    double4_t sum1 = sums[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    double4_t sum2 = y > 0 ? sums[(x+size_x-1) + nx * (y-1)] : zeros;
    
    // second blue square: x at x - 1, y at y + size_y
    double4_t sum3 = x > 0 ? sums[(x-1) + nx * (y + size_y -1)] : zeros;

    // last orange square
    double4_t sum4 = (x > 0 && y > 0) ? sums[(x-1) + nx * (y-1)] : zeros;

    double4_t inner_sum = sum1 - sum2 - sum3 + sum4;
    return inner_sum;
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
    std::vector<double4_t> sums(nx*ny, zeros);

    for (int y = 0; y < ny; ++y) {
        double4_t row_sums_c = zeros;
        for (int x = 0; x < nx; ++x) {
            for (int c = 0; c < 3; ++c) {
                int i = c + 3 * x + 3 * nx * y;
                row_sums_c[c] += data[i];
                row_sums_c[3] += (data[i]*data[i]);
                double sum_i_above = y == 0 ? 0.0 : sums[x + nx*(y-1)][c];
                sums[x+nx*y][c] = sum_i_above + row_sums_c[c];
            }
            double squared_sum_i_above = y == 0 ? 0.0: sums[x + nx*(y-1)][3];
            sums[x + nx*y][3] = squared_sum_i_above + row_sums_c[3];
        }
    }
    double total_sum_of_squares = sums[nx*ny-1][3];

    // Loops for finding min squared error here
    Result empty_result{0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};
    int thread_count = omp_get_max_threads();
    std::vector<double> min_sse(thread_count, total_sum_of_squares); // error is sum_of_squares - stuff, so always less than sum of squares
    std::vector<Result> min_results(thread_count, empty_result); // error is sum_of_squares - stuff, so always less than sum of squares
    #pragma omp parallel for schedule(static, 1)
    for (int size_y = 1; size_y <= ny ; ++size_y) { 
        for (int size_x = 1; size_x <= nx; ++size_x) {
            // inner rectangle cannot be entire rectangle
            if (size_x == nx && size_y == ny) {
                continue;
            }

            // run comparison between rectangles of equal size
            double rec_size = size_y * size_x;
            double outer_size = nx*ny - rec_size;

            for (int y = 0; y <= ny - size_y; ++y) {
                for (int x = 0; x <= nx - size_x; ++x) {
                    // find sum of pixels in / out this rectangle for all color components

                    // find sum of color component in inner / outer rectangle
                    double4_t in_sum = inner_sum(x, y, size_x, size_y, nx, sums);
                    double inner_sums_squares_sum = in_sum[0] * in_sum[0] + in_sum[1] * in_sum[1] + in_sum[2] * in_sum[2];

                    double4_t out_sum = sums[nx*ny - 1] - in_sum;
                    double outer_sums_squares_sum = out_sum[0] * out_sum[0] + out_sum[1] * out_sum[1] + out_sum[2] * out_sum[2];

                    // find sum of squares within this rectangle
                    double inner_sum_of_squares = in_sum[3];
                    double outer_sum_of_squares = total_sum_of_squares - inner_sum_of_squares;

                    // find inner and outer sse
                    double sse_inner = inner_sum_of_squares - ((1.0/rec_size) * inner_sums_squares_sum);
                    double sse_outer = outer_sum_of_squares - ((1.0/outer_size) * outer_sums_squares_sum);
                    double sse = sse_inner + sse_outer;

                    // compare sse to current minimum
                    int thread_id = omp_get_thread_num();
                    if (sse < min_sse[thread_id]) {
                        min_sse[thread_id] = sse;
                        min_results[thread_id] = Result{
                            y, x, y+size_y, x+size_x,
                            {0.0, 0.0, 0.0},
                            {0.0, 0.0, 0.0}
                        };
                    }
                }
            }
        }
    }

    // POSTPROCESSING: find averages minimum location
    double best_sse = total_sum_of_squares;
    int thread_with_best_result = 0;
    for (int thread_i = 0; thread_i < thread_count; ++thread_i) {
        if (min_sse[thread_i] < best_sse) {
            thread_with_best_result = thread_i;
        }
    }
    Result min_result = min_results[thread_with_best_result];

    double min_size_x = min_result.x1 - min_result.x0;
    double min_size_y = min_result.y1 - min_result.y0;
    double rec_size = min_size_x * min_size_y;
    double outer_size = nx*ny - rec_size;

    double4_t in_sum = inner_sum(min_result.x0, min_result.y0 , min_size_x, min_size_y, nx, sums);
    double4_t inner_avgs = in_sum / rec_size;

    double4_t out_sum = sums[nx*ny - 1] - in_sum;
    double4_t outer_avgs = out_sum / outer_size;

    min_result.outer[0] = outer_avgs[0];
    min_result.outer[1] = outer_avgs[1];
    min_result.outer[2] = outer_avgs[2];
    min_result.inner[0] = inner_avgs[0];
    min_result.inner[1] = inner_avgs[1];
    min_result.inner[2] = inner_avgs[2];

    return min_result;
}