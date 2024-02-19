#include <vector>
#include <algorithm>
/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in in[x + y*nx]
- for each pixel (x, y), store the median of the pixels (a, b) which satisfy
  max(x-hx, 0) <= a < min(x+hx+1, nx), max(y-hy, 0) <= b < min(y+hy+1, ny)
  in out[x + y*nx].
*/
void mf(int ny, int nx, int hy, int hx, const float *in, float *out) {
    #pragma omp parallel for schedule(static,1)
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            // pixel location in in & out
            int i = x + y*nx;

            // the sliding window indices
            int wystart = std::max(0, y-hy);
            int wyend = std::min(ny-1, y+hy);
            int wxstart = std::max(0, x-hx);
            int wxend = std::min(nx-1, x+hx);

            // copy sliding window values to vector
            int ysize_window = wyend - wystart + 1;
            int xsize_window = wxend - wxstart + 1;
            int sizeof_window = (ysize_window) * (xsize_window);
            std::vector<double> window(sizeof_window, 0.0);
            for (int j = 0; j < ysize_window; ++j) {
                for (int k = 0; k < xsize_window; ++k) {
                    window[j*xsize_window+k] = in[(wystart+j)*nx+(wxstart+k)];
                }
            }

            // find median from the window
            int median_index = sizeof_window / 2;
            std::nth_element(window.begin(), window.begin() + median_index, window.end());
            double middle_value = window[median_index];

            double median = -1;
            if (sizeof_window % 2 == 0) {
                // re-sort to get value that is right before middle value to obtain average
                std::nth_element(window.begin(), window.begin() + median_index - 1, window.end());
                double before_middle = window[median_index-1];
                median = (before_middle + middle_value) / 2.0;
            } else {
                median = middle_value;
            }
            out[i] = median;
        }
    }
}

