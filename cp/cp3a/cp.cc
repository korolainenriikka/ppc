#include <vector>
#include <cmath>
#include <iostream>
typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));

/**
 * benchmarks:
 * vectorized: Time limit exceeded
 * pragma openmp'd vectors: 12.6 sec
*/

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
    constexpr int nb = 4;
    // vectors per row
    int na = nx % nb == 0 ? nx / nb : (nx/nb)+1;
    bool has_padding = nx % nb != 0;

    // NORMALIZATION

    // CREATE NORMALIZED MATRIX
    // create vector size ny*nx
    std::vector<double4_t> vdata(ny*na);
    std::vector<double4_t> vnorm(ny*na);

    // kopioi datan sisältö vektorimuodon dataan
    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < ny; ++y) {
        for (int a = 0; a < na; ++a) {
            for (int b = 0; b < nb; ++b) {
                // location on the row
                int i = a * nb + b;
                vdata[na*y + a][b] = i < nx ? data[nx*y + i] : 0.0;
            }
        }
    }

    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < ny; ++y) {
        double4_t vsum = {0.0, 0.0, 0.0, 0.0};
        for (int a = 0; a < na; ++a) {
            vsum += vdata[a + y*na];
        }
        double sum = vsum[0]+vsum[1]+vsum[2]+vsum[3];
        double mean = sum/nx;
    
        //vähennä mean jokaisesta elementistä
        for (int a = 0; a < na; ++a){
            int i = a + y*na;
            vnorm[i] = vdata[i] - mean;
        }
    }

    // reset vnorm overflow numbers to 0.0
    int no_of_padding = nb - (nx % nb);
    // vikan vektorin indeksi on na-1
    if (has_padding){
        #pragma omp parallel for schedule(static, 1)
        for (int y = 0; y < ny; ++y) {
            for(int b = nb-no_of_padding; b < nb; ++b) {
                vnorm[(na-1) + na*y][b] = 0.0;
            }
        }
    }

    // normalize the input rows so that for each row the sum of the squares of the elements is 1
    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < ny; ++y) {
        // sos = sum of squares
        double4_t vsos =  {0.0, 0.0, 0.0, 0.0};
        for (int a = 0; a < na; ++a) {
            int i = a + y*na;
            vsos += vnorm[i]*vnorm[i];
        }
        double sum_of_squares = vsos[0]+vsos[1]+vsos[2]+vsos[3];
        double sqrt_sum_of_squares = sqrt(sum_of_squares);

        // jaa sum of squares neliöjuurella jokainen
        for (int a = 0; a < na; ++a) {
            int i = a + y*na;
            vnorm[i] = vnorm[i] / sqrt_sum_of_squares;
        }
    }

    // reset vnorm overflow numbers to 0.0
    if (has_padding){
        for (int y = 0; y < ny; ++y) {
            for(int b = nb-no_of_padding; b < nb; ++b) {
                vnorm[(na-1) + na*y][b] = 0.0;
            }
        }
    }
    
    // MATRIX PRODUCT
    // diagonal elements are always 1
    #pragma omp parallel for schedule(static, 1)
    for (int a = 0; a < ny; ++a) {
        result[a + a*ny] = 1;
    }

    // Calculate the (upper triangle of the) matrix product Y = XX^T
    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < ny; ++y) {
        for (int x = y+1; x < ny; ++x) {
            // akkumuloidaan neljää summaa
            double4_t dot_product_v = {0.0, 0.0, 0.0, 0.0};
            for (int k = 0; k < na; ++k) {
                double4_t tulot = vnorm[k + y*na] * vnorm[k + x*na];
                dot_product_v += tulot;
            }
            double dot_product = dot_product_v[0]+dot_product_v[1]+dot_product_v[2]+dot_product_v[3];
            result[x + y * ny] = dot_product;
        }
    }
}

/**
 * MEMORY ACCESS
 * useless memory accesses for the overflow: fetching zeros from memory max 9000*7 doubles / 9000*9000 doubles, <0.01% ei välii
 * tuloja vektoreiden välillä vois laskee missä vaan järjestyksessä. ei oo välii missä järkässä lasketaan
 *      transpoosi?? vektoreista
 *      16 vektori dot product summaa ja akkumuloi noita kerralla kaikkia.
*/
