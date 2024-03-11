#include <vector>
#include <cmath>
typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));

constexpr int nb = 8;

float vec_sum(float8_t vec) {
    float sum = 0.0;
    for (int i = 0; i<nb; ++i) {
        sum += vec[i];
    }
    return sum;
}

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
    constexpr float8_t zeros = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    };

    // number of columns in normalized matrix with padding
    // vectors per row
    int na = nx % nb == 0 ? nx / nb : (nx/nb)+1;
    bool has_padding = nx % nb != 0;

    // NORMALIZATION

    // CREATE NORMALIZED MATRIX
    // create vector size ny*nx
    std::vector<float8_t> vdata(ny*na);
    std::vector<float8_t> vnorm(ny*na);

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
        float8_t vsum = zeros;
        for (int a = 0; a < na; ++a) {
            vsum += vdata[a + y*na];
        }
        float sum = vec_sum(vsum);
        float mean = sum/nx;
    
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
        float8_t vsos = zeros;
        for (int a = 0; a < na; ++a) {
            int i = a + y*na;
            vsos += vnorm[i]*vnorm[i];
        }
        float sum_of_squares = vec_sum(vsos);
        float sqrt_sum_of_squares = sqrt(sum_of_squares);

        // jaa sum of squares neliöjuurella jokainen
        for (int a = 0; a < na; ++a) {
            int i = a + y*na;
            vnorm[i] = vnorm[i] / sqrt_sum_of_squares;
        }
    }

    // reset vnorm overflow numbers to 0.0
    if (has_padding){
        #pragma omp parallel for schedule(static, 1)
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
    int count_of_squares_y = std::floor((ny / 3) - 1);
    int count_of_squares_x = std::floor(ny / 3);

    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < count_of_squares_y * 3; y += 3) {
        for (int x = (std::floor(y/3) + 1) * 3; x < count_of_squares_x * 3; x += 3) {
            // nine sums first index: y second index: x
            float8_t dp_vecs[3][3];
            for (int id = 0; id < 3; ++id) {
                for (int jd = 0; jd < 3; ++jd) {
                    dp_vecs[id][jd] = zeros;
                }
            }

            for (int k = 0; k < na; ++k) {
                float8_t y0 = vnorm[na*(y + 0) + k];
                float8_t y1 = vnorm[na*(y + 1) + k];
                float8_t y2 = vnorm[na*(y + 2) + k];
                float8_t x0 = vnorm[na*(x + 0) + k];
                float8_t x1 = vnorm[na*(x + 1) + k];
                float8_t x2 = vnorm[na*(x + 2) + k];
                dp_vecs[0][0] += y0 * x0;
                dp_vecs[0][1] += y0 * x1;
                dp_vecs[0][2] += y0 * x2;
                dp_vecs[1][0] += y1 * x0;
                dp_vecs[1][1] += y1 * x1;
                dp_vecs[1][2] += y1 * x2;
                dp_vecs[2][0] += y2 * x0;
                dp_vecs[2][1] += y2 * x1;
                dp_vecs[2][2] += y2 * x2;
            }
            for (int yd = 0; yd < 3; ++yd) {
                for (int xd = 0; xd < 3; ++xd) {
                    // me ollaan resultissa niin että ylävasen on rivi y sarake x
                    // se on resultissa kohta y*ny + x
                    // mutta resultin sisällä ollaan rivillä yd ja sarakkeella xd
                    // isossa matriisissa se on rivi y+yd sarake x+xd
                    // eli se on resultsissa kohta (y+yd)*ny +(x+xd)
                    result[(y+yd)*ny +(x+xd)] = vec_sum(dp_vecs[yd][xd]);
                }
            }
        }
    }

    // OIKEALLE JÄÄVÄT TEKEMÄTTÖMÄT 0 1 tai 2 SARAKETTA
    int no_of_leftover_cols = ny%3;
    for (int y = 0; y < ny; ++y) {
        for (int x_leftover = 0; x_leftover < no_of_leftover_cols; ++x_leftover) {
            int x = count_of_squares_x*3 + x_leftover;

            float8_t dot_product_v = zeros;
            for (int k = 0; k < na; ++k) {
                float8_t tulot = vnorm[k + y*na] * vnorm[k + x*na];
                dot_product_v += tulot;
            }
            float dot_product = vec_sum(dot_product_v);
            result[x + y * ny] = dot_product;
        }
    }

    // KESKELLE JÄÄVÄT TEKEMÄTTÖMÄT PIKKUKOLMIOT
    #pragma omp parallel for schedule(static, 1)
    for (int y = 0; y < (count_of_squares_y + 1) * 3; y += 3) {
        int x = y;
        float8_t dp_vecs[3];
        for (int id = 0; id < 3; ++id) {
            dp_vecs[id] = zeros;
        }

        for (int k = 0; k < na; ++k) {
            float8_t y0 = vnorm[na*(y + 0) + k];
            float8_t y1 = vnorm[na*(y + 1) + k];
            float8_t x1 = vnorm[na*(x + 1) + k];
            float8_t x2 = vnorm[na*(x + 2) + k];
            dp_vecs[0] += y0 * x1;
            dp_vecs[1] += y0 * x2;
            dp_vecs[2] += y1 * x2;
        }
        result[(y+0)*ny +(x+1)] = vec_sum(dp_vecs[0]); // yd = 0 xd = 1
        result[(y+0)*ny +(x+2)] = vec_sum(dp_vecs[1]); // yd = 0 xd = 2
        result[(y+1)*ny +(x+2)] = vec_sum(dp_vecs[2]); // yd = 1 xd = 2
    }
}