// Tilanne:
// looppaaminen onnistuu mutta: 
// jos mulla on koordinaatit x y ja c ja muistipaikka mistä data alkaa,
// miten pääsen haluttuun kohtaan dataa?
// eli missä järjestyksessä data on muistissa ja mistä tiedän missä järjestyksessä se on?

#include <iostream>
#include <vector>

using namespace std;

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
    float a = *data;
    float r = a*0.0;
    float g = 0.0;
    float b = 0.0;

    // looppaa jokainen rivi kuvassa
    for (int x = x0; x < x1-x0; ++x) {
        // looppaa jokainen sarake
        for (int y = y0; y < y1-y0; ++y) {
            // looppaa jokainen rgb
            //for (int c = 0; c < 3; ++c) {
                //cout << data[c + 3 * x + 3 * nx * y];
            r += 1;
            g += 1;
            b += 1;
            //}
        }
    }

    // divide by number of pixels (nx * ny)
    r = r / nx*ny;
    g = g / nx*ny;
    b = b / nx*ny;

    // create result
    Result result{{r, g, b}};
    return result;
}

// int main()
// {
//     int ny = 1;
//     int nx = 1;
//     const vector<vector<float>> data = {
//         {
//             {1.0f, 1.0f, 1.0f}
//         },
//         {
//             {2.0f, 3.0f, 4.0f}
//         }
//     };

//     int y0 = 0;
//     int x0 = 0;
//     int y1 = 2;
//     int x1 = 1;

//     Result res = calculate(ny, nx, &data[0][0], y0, x0, y1, x1);
//     //cout << res << "\n";
// }
