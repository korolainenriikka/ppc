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
    std::vector<double> sums_c(3, 0.0);
    std::vector<double> squared_sums(3*nx*ny, 0.0);
    std::vector<double> squared_sums_c(3, 0.0);
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            for (int c = 0; c < 3; ++c) {
                int i = c + 3 * x + 3 * nx * y;
                sums_c[c] = sums_c[c] + data[i];
                sums[i] = sums_c[c];
                squared_sums_c[c] = squared_sums_c[c] + (data[i]*data[i]);
                squared_sums[i] = squared_sums_c[c];
            }
        }
    }


    // PREPROCESSING: create size ny*nx array
    // each pixel has sum of absolute errors for rectangle being from top left to this point for outer and inner
    // TÄS EI SALEE OO JÄRKEE
    // std::vector<double> sabses(nx*ny, 0.0);
    // std::vector<double> avgs(3*nx*ny, 0.0);
    // for (int y = 0; y < ny; ++y) {
    //     for (int x = 0; x < nx; ++x) {
    //         // find averages for each color
    //         int inner_size = (x+1) * (y+1);
    //         for (int c = 0; c < 3; ++c) {
    //             int i = c + 3 * x + 3 * nx * y;
    //             double sum_inner_c = sums[i];
    //             avgs[i] = sum_inner_c / inner_size;
    //         }

    //         // find sum of absolute errors for this rectangle
    //         // std::cout << "new partition\n";
    //         for (int yy = 0; yy <= y; ++yy) {
    //             for (int xx = 0; xx <= x; ++xx) {
    //                 for (int c = 0; c < 3; ++c) {
    //                     int i = c + 3 * x + 3 * nx * y;
    //                     sabses[x + nx * y] += std::abs(data[i]-avgs[i]);
    //                     // std::cout << "pixel" << yy << ' ' << xx << ' ' << c <<' ' << "average: " << avg << " value of component" << val << " error " << std::abs(val-avg) << '\n';
    //                 }
    //             }
    //         }
    //     }
    // }

    // Loops for finding min squared error here
    Result result{0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};
    double full_cost = sabses[nx*ny -1];
    double min_error = 100;
    for (int size_y = 1; size_y <= ny ; ++size_y) { 
        for (int size_x = 1; size_x <= nx; ++size_x) {
            // inner rectangle cannot be entire rectangle
            if (size_x == nx && size_y == ny) {
                return result;
            }

            // run comparison between rectangles of equal size!

            for (int y = 0; y < ny; ++y) {
                for (int x = 0; x < nx; ++x) {
                    // std::cout << "checking case. size x " << size_x << " size y " << size_y << " pos y " << y << " and x " << x << '\n'; indeksitarkistus

                    // until here corner
                    double uhcorner = sabses[x + nx*y]; // x,y
                    double inner_cost = uhcorner;
                    double outer_cost = full_cost - inner_cost;
                    // ONGELMA: edeltävien perusteella cost on aina full_cost.
                    // upper margin: x, y-size_y
                    // int uppermargin 

                    // left margin: y - x-size_x
                    // rectangle
                    // outer: entire - rectangle
                }
            }
        }
    }

    return result;
}
