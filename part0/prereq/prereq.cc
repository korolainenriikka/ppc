struct Result {
    float avg[3];
};

/*
This is the function you need to implement. Quick reference:
- x coordinates: 0 <= x < nx
- y coordinates: 0 <= y < ny
- horizontal position: 0 <= x0 < x1 <= nx
- vertical position: 0 <= y0 < y1 <= ny
- color components: 0 <= c < 3
- input: data[c + 3 * x + 3 * nx * y]
- output: avg[c]
*/
Result calculate(int ny, int nx, const float *data, int y0, int x0, int y1, int x1) {
    // find sum of each component
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            r += data[0 + 3 * x + 3 * nx * y];
            g += data[1 + 3 * x + 3 * nx * y];
            b += data[2 + 3 * x + 3 * nx * y];
        }
    }

    // divide by number of pixels
    int count_of_pixels = (y1-y0)*(x1-x0);
    float r_avg = r / count_of_pixels;
    float g_avg = g / count_of_pixels;
    float b_avg = b / count_of_pixels;

    // create result
    Result res = Result{{r_avg, g_avg, b_avg}};
    return res;
}

