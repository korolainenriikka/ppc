#include <iostream>
#include <vector>

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
double inner_sum(int x, int y, int c, int size_x, int size_y, int nx, std::vector<double>& sums) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = c + 3 * (x+size_x-1) + 3 * nx * (y+size_y-1);
    double sum1 = sums[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    double sum2 = y > 0 ? sums[c + 3 * (x+size_x-1) + 3 * nx * (y-1)] : 0;
    
    // second blue square: x at x - 1, y at y + size_y
    double sum3 = x > 0 ? sums[c + 3 * (x-1) + 3 * nx * (y + size_y -1)] : 0;

    // last orange square
    double sum4 = (x > 0 && y > 0) ? sums[c + 3 * (x-1) + 3 * nx * (y-1)] : 0;

    double inner_sum = sum1 - sum2 - sum3 + sum4;
    return inner_sum;
}

double inner_sos(int x, int y, int size_x, int size_y, int nx, std::vector<double>& sos) {
    // first orange square: x at x + size_x - 1,  y at y + size_y - 1 (-1 bc size starts from 1)
    int sum1_i = (x+size_x-1) + nx * (y+size_y-1);
    double sum1 = sos[sum1_i];

    // first blue square: x at x + size_x, y at y-1
    double sum2 = y > 0 ? sos[(x+size_x-1) + nx * (y-1)] : 0;
    
    // second blue square: x at x - 1, y at y + size_y
    double sum3 = x > 0 ? sos[(x-1) + nx * (y + size_y -1)] : 0;

    // last orange square
    double sum4 = (x > 0 && y > 0) ? sos[(x-1) + nx * (y-1)] : 0;

    double inner_sum = sum1 - sum2 - sum3 + sum4;
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
    std::vector<double> sums(3*nx*ny, 0.0);
    std::vector<double> squared_sums(nx*ny, 0.0);

    for (int y = 0; y < ny; ++y) {
        std::vector<double> row_sums_c(3, 0.0);
        double row_sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            for (int c = 0; c < 3; ++c) {
                int i = c + 3 * x + 3 * nx * y;
                row_sums_c[c] += data[i];
                row_sum_of_squares += (data[i]*data[i]);
                double sum_i_above = y == 0 ? 0.0 : sums[c + 3*x + 3*nx*(y-1)];
                sums[i] = sum_i_above + row_sums_c[c];
            }
            double squared_sum_i_above = y == 0 ? 0.0 : squared_sums[x + nx*(y-1)];
            squared_sums[x + nx*y] = squared_sum_i_above + row_sum_of_squares;
        }
    }
    double total_sum_of_squares = squared_sums[nx*ny-1];

    // Loops for finding min squared error here
    Result result{0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};
    double min_sse = total_sum_of_squares; // error is sum_of_squares - stuff, so always less than sum of squares
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
                    // std::cout << "checking case. size x " << size_x << " size y " << size_y << " pos y " << y << " and x " << x << '\n'; // indeksitarkistus
                    
                    // find sum of pixels in / out this rectangle for all color components
                    double inner_sums_squares_sum = 0.0;
                    double outer_sums_squares_sum = 0.0;
                    for (int c = 0; c < 3; ++c) {
                        // find sum of color component in inner / outer rectangle
                        double in_sum = inner_sum(x, y, c, size_x, size_y, nx, sums);
                        inner_sums_squares_sum += in_sum * in_sum;

                        double out_sum = sums[3*nx*ny + (c-3)] - in_sum;
                        outer_sums_squares_sum += out_sum * out_sum; // sums_c has the sums of the full image stored

                        // // verify that sums are correct
                        // std::cout << " sum of inner pixels: " << sum_inner;
                        // std::cout << ", sum of outer pixels: " << sum_outer <<'\n';
                    }

                    // find sum of squares within this rectangle
                    double inner_sum_of_squares = inner_sos(x, y, size_x, size_y, nx, squared_sums);
                    double outer_sum_of_squares = total_sum_of_squares - inner_sum_of_squares;

                    // find inner and outer sse
                    double sse_inner = inner_sum_of_squares - ((1.0/rec_size) * inner_sums_squares_sum);
                    double sse_outer = outer_sum_of_squares - ((1.0/outer_size) * outer_sums_squares_sum);
                    double sse = sse_inner + sse_outer;

                    // compare sse to current minimum
                    if (sse < min_sse) {
                        min_sse = sse;
                        std::vector<float> inner_avgs(3, 0.0);
                        std::vector<float> outer_avgs(3, 0.0);
                        for (int c = 0; c < 3; ++c) {
                            double in_sum = inner_sum(x, y, c, size_x, size_y, nx, sums);
                            inner_avgs[c] = in_sum / rec_size;

                            double out_sum = sums[3*nx*ny + (c-3)] - in_sum;
                            outer_avgs[c] = out_sum / outer_size;
                        }
                        result = Result{
                            y, x, y+size_y, x+size_x,
                            {outer_avgs[0],outer_avgs[1],outer_avgs[2]},
                            {inner_avgs[0],inner_avgs[1],inner_avgs[2]}
                        };
                    }
                }
            }
        }
    }

    return result;
}