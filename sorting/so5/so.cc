#include <algorithm>
#include <iostream>
#include <vector>
#include <omp.h>

typedef unsigned long long data_t;

int pivot(int p, int end, data_t* data) {
    // choose random index between p and ent
    int pivot_i = end == 1 ? p : (std::rand() % (end-p) + p);
    std::swap(data[p], data[pivot_i]);

    int r = p;

    int q = p+1;
    int j = p+1;
    p = p+1;

    data_t pivot = data[r];
    while (j < end) {
        if (data[j] < pivot) {
            std::swap(data[j], data[q]);
            q += 1;
        }
        j += 1;
    }

    std::swap(data[r], data[q-1]);
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
    // for (int i = 0; i < len_qs; ++i) {
    //     std::cout << qs[i] << ' ';
    // }

    #pragma omp parallel for schedule(static, 1)
    for (int i = 1; i < len_qs; ++i){
        std::sort(data + qs[i-1], data + qs[i]);
    }
}
