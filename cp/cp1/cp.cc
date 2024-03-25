#include <iostream>
#include <cmath>
#include <vector>

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/

void correlate(int ny, int nx, const float *data, float *result) {
    // NORMALIZATION
    std::vector<double> normalized(ny*nx, 0.0);

    // vähennä mean jokaisesta elementistä
    for (int y = 0; y < ny; ++y) {
        double sum = 0;
        for (int x = 0; x < nx; ++x) {
            sum += data[x + y*nx];
        }
        double mean = sum/nx;
    
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
        double sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            sum_of_squares += normalized[x + y*nx]*normalized[x + y*nx];
        }
        double sqrt_sum_of_squares = sqrt(sum_of_squares);

        for (int x = 0; x < nx; ++x) {
            normalized[x + y*nx] = normalized[x + y*nx] / sqrt_sum_of_squares;
        }
    }
    
    // Calculate the (upper triangle of the) matrix product Y = XX^T
    for (int y = 0; y < ny; ++y) {
        for (int x = y; x < ny; ++x) {
            double dot_product = 0.0;
            for (int k = 0; k < nx; ++k) {
                dot_product += normalized[k + y*nx] * normalized[k + x*nx];
            }
            result[x + y * ny] = dot_product;
        }
    }
}