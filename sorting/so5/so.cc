#include <algorithm>
#include <iostream>

typedef unsigned long long data_t;

void psort(int n, data_t *data) {
    if (n==0) {
        return;
    }
    int r = n-1;
    int p = 0;
    int q = p;
    int j = p;

    data_t pivot = data[r];
    // std::cout << pivot << '\n';
    while (j < r) {
        if (data[j] < pivot) {
            data_t swp = data[j];
            data[j] = data[q];
            data[q] = swp;
            q += 1;
        }
        j += 1;
    }

    data[r] = data[q];
    data[q] = pivot;

    #pragma omp sections
    {
        #pragma omp section
        std::sort(data, data + q);
        #pragma omp section
        std::sort(data + q, data + n);
    }
}
