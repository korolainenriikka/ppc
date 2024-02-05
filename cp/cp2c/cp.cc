#include <vector>
#include <cmath>
#include <iostream>
// baseline speed (local)
// benchmarks/2.txt     3.792s  pass

typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));
constexpr float infty = std::numeric_limits<float>::infinity();

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

    // CREATE NORMALIZED MATRIX
    // create vector size ny*nx
    // vectorized: nx should be divisible by 4
    int row_length = nx + (4-(nx%4));
    std::vector<double> normalized(ny*nx, 0.0);

    for (int y = 0; y < ny; ++y) {
        double sum = 0;
        for (int x = 0; x < nx; ++x) {
            sum += data[x + y*nx];
        }
        double mean = sum/nx;
        //std::cout << mean << " ";
    
        // vähennä mean jokaisesta elementistä
        for (int x = 0; x < nx; ++x) {
            if (mean != 0) {
                normalized[x + y*nx] = data[x + y*nx] - mean;
            } else {
                normalized[x + y*nx] = data[x + y*nx];
            }
        }
    }

    // elements per vector
    constexpr int nb = 4;
    // vectors per row
    int na = nx / nb;
    
    // normalize the input rows so that for each row the sum of the squares of the elements is 1
    for (int y = 0; y < ny; ++y) {
        double sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            sum_of_squares += normalized[x + y*row_length]*normalized[x + y*row_length];
        }
        // jaa sum of squares neliöjuurella jokainen
        double sqrt_sum_of_squares = sqrt(sum_of_squares);
        std::cout << sqrt_sum_of_squares;

        for (int x = 0; x < na; ++x) {
            //for (int z = 0; z < nb; ++z) {
            //int index = z + z * x + y*nx;
            int index = x + y*nx;
            normalized[index] = index >= nx ? infty : normalized[index] / sqrt_sum_of_squares;
            //}
        }
    }
    
    // MATRIX PRODUCT
    for (int a = 0; a < ny; ++a) {
        result[a + a*ny] = 1;
    }


    // VECTORIZATION
    // add padding to rows: x-dimension should be divisible by 4

    // print normalized
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < row_length; ++x) {
            std::cout << normalized[x + y*nx] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // accumulate 4 sums of multiplications --> sum together before result = dot product
    
    // for (int y = 0; y < ny; ++y) {
    //     for (int x = y+1; x < ny; ++x) {
    //         double dot_product = 0.0;
    //         double4_t dot_product_v = 0.0
    //         for (int k = 0; k < nx; ++k) {
    //             dot_product += normalized[k + y*nx] * normalized[k + x*nx];
    //         }
    //         //result[x + y * ny] = dot_product;
    //     }
    // }
}
