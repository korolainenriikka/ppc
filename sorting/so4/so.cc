#include <algorithm>
#include <iostream>
#include <omp.h>
#include <vector>

typedef unsigned long long data_t;

using namespace std;

void psort(int n, data_t *data) {
    // FIXME: Implement a more efficient parallel sorting algorithm for the CPU,
    // using the basic idea of merge sort.
    // eka idea: jaa data max-threads määrään subarrayita, sorttaa subarrayt
    // sit mergee ne yhteen
    int ts = omp_get_max_threads();

    // divide array into t subarrays
    int subarr_size = n / ts;
    if (subarr_size == 0){
        std::sort(data, data + n);
        return;
    }

    // parallelly sort subarrays
    vector<data_t> helper(n, 0);
    vector<int> subarr_pointers(ts,0);
    #pragma omp parallel for
    for (int t = 0; t < ts; ++t) {
        int arr_start = subarr_size * t;
        int arr_end = t == ts-1 ? n : arr_start + subarr_size;
        std::sort(data + arr_start, data + arr_end);
        // copy sorted values to helper
        for (int i = arr_start; i < arr_end; ++i) {
            helper[i] = data[i];
        }
        subarr_pointers[t] = arr_start;
    }

    // merge subarrays

    // maintain t pointers to subarray indices, find subarray end indices
    vector<int> arr_ends(ts,0);
    for (int t = 0; t < ts; ++t) {
        arr_ends[t] = t == ts-1 ? n : subarr_size * t + subarr_size;
    }

    for (int i = 0; i < n; ++i) {
        // cout << "data: \n";
        // for (int j = 0; j < 21; ++j) {
        //     cout << data[j]<<',';
        // }
        // cout << '\n';


        // find minimum of subarray starts
        data_t min_number = 0;
        int min_thread = -1;
        for (int t = 0; t < ts; ++t) {
            // -1 means that all values from this subarray have been taken
            if (subarr_pointers[t] != -1) {
                data_t current = helper[subarr_pointers[t]]; 
                if (min_thread == -1 || current < min_number) {
                    min_number = current;
                    min_thread = t;
                }
            }
        }

        if(min_thread == -1) {
            break;
        }

        // write smallest number to data
        data[i] = min_number;

        // increment pointer
        int next_loc = subarr_pointers[min_thread] + 1;
        // if thread has iterated all subarray values, remove thread from subarr pointers (mark with -1)
        // thread i indeksi loppuu kohdassa
        if (next_loc == arr_ends[min_thread]) {
            next_loc = -1;
        }
        subarr_pointers[min_thread] = next_loc;
    }
}
