#include <vector>
#include <cmath>
#include <iostream>
// baseline speed (local)
// benchmarks/2.txt     3.792s  pass

// test 1 correct normalized
//-0.707107 0.707107 
//-0.707107 0.707107

typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
    // number of columns in normalized matrix with padding
    //int ncols = nx + (4-nx%4);
    // vec size
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
    for (int y = 0; y < ny; ++y) {
        for (int a = 0; a < na; ++a) {
            for (int b = 0; b < nb; ++b) {
                // location on the row
                int i = a * nb + b;
                vdata[na*y + a][b] = i < nx ? data[nx*y + i] : 0.0;
            }
        }
    }

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
        for (int y = 0; y < ny; ++y) {
            for(int b = nb-no_of_padding; b < nb; ++b) {
                vnorm[(na-1) + na*y][b] = 0.0;
            }
        }
    }

    // normalize the input rows so that for each row the sum of the squares of the elements is 1
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
    for (int a = 0; a < ny; ++a) {
        result[a + a*ny] = 1;
    }

    // Calculate the (upper triangle of the) matrix product Y = XX^T
    for (int y = 0; y < ny; ++y) {
        for (int x = y+1; x < ny; ++x) {
            //yth row and xth row
            // akkumuloidaan neljää summaa
            double4_t dot_product_v = {0.0, 0.0, 0.0, 0.0};
            for (int k = 0; k < na; ++k) {
                // rows i and j in index k
                // luo vektori mis on neljä tuloo
                // rivi x ja y
                double4_t tulot = vnorm[k + y*na] * vnorm[k + x*na];
                dot_product_v += tulot;
            }
            double dot_product = dot_product_v[0]+dot_product_v[1]+dot_product_v[2]+dot_product_v[3];
            //std::cout <<dot_product;
            result[x + y * ny] = dot_product;
        }
    }
}

    // for (int y = 0; y < ny; ++y) {
    //     for (int a = 0; a < na; ++a) {
    //         std::cout << vnorm[a + na*y][0]<<' '; 
    //         std::cout << vnorm[a + na*y][1]<<' ';  
    //         std::cout << vnorm[a + na*y][2] <<' '; 
    //         std::cout << vnorm[a + na*y][3] << '\n'; 
    //     }
    // }