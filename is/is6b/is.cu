#include <iostream>
#include <vector>
#include <omp.h>

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
float inner_sum(int x, int y, int c, int size_x, int size_y, int nx, std::vector<float>& sums) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = c + 3 * (x+size_x-1) + 3 * nx * (y+size_y-1);
    float sum1 = sums[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    float sum2 = y > 0 ? sums[c + 3 * (x+size_x-1) + 3 * nx * (y-1)] : 0;
    
    // second blue square: x at x - 1, y at y + size_y
    float sum3 = x > 0 ? sums[c + 3 * (x-1) + 3 * nx * (y + size_y -1)] : 0;

    // last orange square
    float sum4 = (x > 0 && y > 0) ? sums[c + 3 * (x-1) + 3 * nx * (y-1)] : 0;

    float inner_sum = sum1 - sum2 - sum3 + sum4;
    return inner_sum;
}

float inner_sos(int x, int y, int size_x, int size_y, int nx, std::vector<float>& sos) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = (x+size_x-1) + nx * (y+size_y-1);
    float sum1 = sos[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    float sum2 = y > 0 ? sos[(x+size_x-1) + nx * (y-1)] : 0;
    
    // second blue square: x at x - 1, y at y + size_y
    float sum3 = x > 0 ? sos[(x-1) + nx * (y + size_y -1)] : 0;

    // last orange square
    float sum4 = (x > 0 && y > 0) ? sos[(x-1) + nx * (y-1)] : 0;

    float inner_sum = sum1 - sum2 - sum3 + sum4;
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
    std::vector<float> sums(3*nx*ny, 0.0);
    std::vector<float> squared_sums(nx*ny, 0.0);

    for (int y = 0; y < ny; ++y) {
        std::vector<float> row_sums_c(3, 0.0);
        float row_sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            for (int c = 0; c < 3; ++c) {
                int i = c + 3 * x + 3 * nx * y;
                row_sums_c[c] += data[i];
                row_sum_of_squares += (data[i]*data[i]);
                float sum_i_above = y == 0 ? 0.0 : sums[c + 3*x + 3*nx*(y-1)];
                sums[i] = sum_i_above + row_sums_c[c];
            }
            float squared_sum_i_above = y == 0 ? 0.0 : squared_sums[x + nx*(y-1)];
            squared_sums[x + nx*y] = squared_sum_i_above + row_sum_of_squares;
        }
    }
    float total_sum_of_squares = squared_sums[nx*ny-1];

    // Loops for finding min squared error here
    Result empty_result{0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};
    std::vector<float> min_sse(omp_get_max_threads(), total_sum_of_squares); // error is sum_of_squares - stuff, so always less than sum of squares
    std::vector<Result> min_results(omp_get_max_threads(), empty_result); // error is sum_of_squares - stuff, so always less than sum of squares
    #pragma omp parallel for
    for (int size_y = 1; size_y <= ny ; ++size_y) { 
        for (int size_x = 1; size_x <= nx; ++size_x) {
            // inner rectangle cannot be entire rectangle
            if (size_x == nx && size_y == ny) {
                continue;
            }

            // run comparison between rectangles of equal size
            float rec_size = size_y * size_x;
            float outer_size = nx*ny - rec_size;

            for (int y = 0; y <= ny - size_y; ++y) {
                for (int x = 0; x <= nx - size_x; ++x) {
                    // std::cout << "checking case. size x " << size_x << " size y " << size_y << " pos y " << y << " and x " << x << '\n'; // indeksitarkistus
                    
                    // find sum of pixels in / out this rectangle for all color components
                    float inner_sums_squares_sum = 0.0;
                    float outer_sums_squares_sum = 0.0;
                    for (int c = 0; c < 3; ++c) {
                        // find sum of color component in inner / outer rectangle
                        float in_sum = inner_sum(x, y, c, size_x, size_y, nx, sums);
                        inner_sums_squares_sum += in_sum * in_sum;

                        float out_sum = sums[3*nx*ny + (c-3)] - in_sum;
                        outer_sums_squares_sum += out_sum * out_sum; // sums_c has the sums of the full image stored

                        // // verify that sums are correct
                        // std::cout << " sum of inner pixels: " << sum_inner;
                        // std::cout << ", sum of outer pixels: " << sum_outer <<'\n';
                    }

                    // find sum of squares within this rectangle
                    float inner_sum_of_squares = inner_sos(x, y, size_x, size_y, nx, squared_sums);
                    float outer_sum_of_squares = total_sum_of_squares - inner_sum_of_squares;

                    // find inner and outer sse
                    float sse_inner = inner_sum_of_squares - ((1.0/rec_size) * inner_sums_squares_sum);
                    float sse_outer = outer_sum_of_squares - ((1.0/outer_size) * outer_sums_squares_sum);
                    float sse = sse_inner + sse_outer;

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
    int thread_with_best_result = std::distance(std::begin(min_sse), std::min_element(std::begin(min_sse), std::end(min_sse)));
    Result min_result = min_results[thread_with_best_result];
    
    std::vector<float> inner_avgs(3, 0.0);
    float outer_avgs[3] = {0.0, 0.0, 0.0};

    float min_size_x = min_result.x1 - min_result.x0;
    float min_size_y = min_result.y1 - min_result.y0;
    float rec_size = min_size_x * min_size_y;
    float outer_size = nx*ny - rec_size;
    for (int c = 0; c < 3; ++c) {
        float in_sum = inner_sum(min_result.x0, min_result.y0, c, min_size_x, min_size_y, nx, sums);
        inner_avgs[c] = in_sum / rec_size;

        float out_sum = sums[3*nx*ny + (c-3)] - in_sum;
        outer_avgs[c] = out_sum / outer_size;
    }
    min_result.outer[0] = outer_avgs[0];
    min_result.outer[1] = outer_avgs[1];
    min_result.outer[2] = outer_avgs[2];
    min_result.inner[0] = inner_avgs[0];
    min_result.inner[1] = inner_avgs[1];
    min_result.inner[2] = inner_avgs[2];

    return min_result;
}