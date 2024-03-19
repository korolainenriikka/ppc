#include <algorithm>
#include <iostream>
#include <vector>
#include <omp.h>

typedef unsigned long long data_t;

int pivot(int p, int end, data_t* data) {
    // test: first element as pivot
    int r = p;

    int q = p+1;
    int j = p+1;
    p = p+1;

    data_t pivot = data[r];
    while (j < end) {
        if (data[j] < pivot) {
            data_t swp = data[j];
            data[j] = data[q];
            data[q] = swp;
            q += 1;
        }
        j += 1;
    }

    data[r] = data[q-1];
    data[q-1] = pivot;
    return q-1;
}

void psort(int n, data_t* data) {
    if (n==0) {
        return;
    }
    int num_threads = omp_get_max_threads();
    int len_qs = num_threads + 1;
    std::vector<int> qs(len_qs,n);
    qs[0] = 0;

    // zero and n always included
    int num_qs = 2;
    while (num_qs < len_qs-1) {
        // find largest section start and end
        int start = 0;
        int end = 0; // end exclusive
        for (int i = 0; i < len_qs-1; ++i) {
            if (qs[i] == n) {
                break;
            }
            int sec_start = qs[i];
            int sec_end = qs[i+1];
            if (sec_end-sec_start > end - start) {
                start = sec_start;
                end = sec_end;
            }
        }
        // pivot: remove last element
        int new_pivot = pivot(start, end, data);
        qs[num_qs-1] = new_pivot;
        std::sort(qs.begin(), qs.begin()+len_qs);
        ++num_qs;
    }
    for (int i = 0; i < len_qs; ++i) {
        std::cout << qs[i] << ' ';
    }

    #pragma omp parallel for schedule(static, 1)
    for (int i = 1; i < len_qs; ++i){
        std::sort(data + qs[i-1], data + qs[i]);
    }
}
