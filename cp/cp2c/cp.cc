#include <vector>
#include <cmath>
#include <iostream>
// baseline speed (local)
// benchmarks/2.txt     3.792s  pass

// test 1 correct normalized
//-0.707107 0.707107 
//-0.707107 0.707107

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
    int ncols = nx + (4-nx%4);
    std::vector<double> normalized(ny*ncols, 0.0);

    for (int y = 0; y < ny; ++y) {
        double sum = 0;
        for (int x = 0; x < nx; ++x) {
            sum += data[x + y*nx];
        }
        double mean = sum/nx;
        //std::cout << mean << " ";
    
        // vähennä mean jokaisesta elementistä
        for (int x = 0; x < nx; ++x){
            int i = x + y*nx;
            int i_norm = x + y*ncols;
            if (mean != 0) {
                normalized[i_norm] = data[i] - mean;
            } else {
                normalized[i_norm] = data[i];
            }
        }
        //std::cout << "\n";
    }

    // normalize the input rows so that for each row the sum of the squares of the elements is 1
    for (int y = 0; y < ny; ++y) {
        double sum_of_squares = 0.0;
        for (int x = 0; x < nx; ++x) {
            int i = x + y*ncols;
            sum_of_squares += normalized[i]*normalized[i];
        }
        // jaa sum of squares neliöjuurella jokainen
        double sqrt_sum_of_squares = sqrt(sum_of_squares);

        for (int x = 0; x < nx; ++x) {
            int i = x + y*ncols;
            normalized[i] = normalized[i] / sqrt_sum_of_squares;
        }
    }
    
    // MATRIX PRODUCT
    // diagonal elements are always 1
    for (int a = 0; a < ny; ++a) {
        result[a + a*ny] = 1;
    }

    // print normalized
    // for (int y = 0; y < ny; ++y) {
    //     for (int x = 0; x < nx; ++x) {
    //         std::cout << normalized[x + y*nx] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\n";

    // Calculate the (upper triangle of the) matrix product Y = XX^T
    for (int y = 0; y < ny; ++y) {
        for (int x = y+1; x < ny; ++x) {
            //std::cout << y;
            //std::cout << x;
            //yth row and xth row
            double dot_product = 0.0;
            for (int k = 0; k < nx; ++k) {
                // rows i and j in index k
                //std::cout << normalized[k + y*nx] << " " << normalized[k + x*nx] << "\n";
                dot_product += normalized[k + y*ncols] * normalized[k + x*ncols];
            }
            //std::cout <<dot_product;
            result[x + y * ny] = dot_product;
        }
    }

    // independent operations
    // multiplications of components of the dot product
    // computations of individual dot products
}
