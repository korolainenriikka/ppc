#include <algorithm>
#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <math.h>

typedef unsigned long long data_t;

using namespace std;

void psort(int n, data_t *data) {
    // FIXME: Implement a more efficient parallel sorting algorithm for the CPU,
    // using the basic idea of merge sort.
    // nyt: mergee parallelisoidusti aina 2 subarrayta kerrallaan
    // --> subarrayiden määrä puolittuu per iteraatio
    int ts = omp_get_max_threads();
    // auto started = std::chrono::high_resolution_clock::now();

    // divide array into t subarrays
    int subarr_size = n / ts;
    if (subarr_size == 0){
        std::sort(data, data + n);
        return;
    }

    // parallelly sort subarrays
    vector<data_t> helper(n, 0);
    vector<data_t> helper2(n, 0);
    vector<int> subarr_pointers(ts,0);
    #pragma omp parallel for
    for (int t = 0; t < ts; ++t) {
        int arr_start = subarr_size * t;
        int arr_end = t == ts-1 ? n : arr_start + subarr_size;
        std::sort(data + arr_start, data + arr_end);
        // copy sorted values to helper
        for (int i = arr_start; i < arr_end; ++i) {
            helper[i] = data[i];
            helper2[i] = data[i];
        }
        subarr_pointers[t] = arr_start;
    }

    // MERGE
    int merge_no = 0; // for following read / write locations
    int num_subarrays = ts;
    while (num_subarrays > 1) {
        // read / write location: alternate between data and helper
        auto read = merge_no % 2 == 0 ? begin(helper) : begin(helper2);
        auto write = merge_no % 2 == 0 ? begin(helper2) : begin(helper);
        cout << "MERGE NUMBER " << merge_no <<'\n';

        int merge_threads = num_subarrays / 2;
        cout << "Merging " << num_subarrays << " subarrays\n";
        cout << "current order: \n";
        for (int i = 0; i < n; ++i) {
            cout << read[i] <<' ';
        }
        cout << '\n';
        // merge 2 subarrays at a time
        int subarr_length = subarr_size * pow(2, merge_no);
        #pragma omp parallel for
        for (int m = 0; m < merge_threads; ++m) {
            cout << "Merging two subarrays\n";
            auto first_arr_p = m * subarr_length * 2; // 2 bc we handle 2 sub-arrays at a time
            auto first_arr_ends_p = m * subarr_length * 2 + subarr_length;
            auto second_arr_p = m * subarr_length * 2 + subarr_length;
            auto arrs_end_p = m == merge_threads -1 ? n : second_arr_p + subarr_length;
            cout << first_arr_p << ' ' << second_arr_p << ' ' << arrs_end_p <<'\n';

            for (int i = first_arr_p; i < arrs_end_p; ++i) {
                cout << "finding value to write to " << i <<'\n';
                data_t first_val = read[first_arr_p];
                data_t second_val = read[second_arr_p];

                if (first_val < second_val) {
                    cout << "first was smaller, writing " << first_val << " from first_pointer at " << first_arr_p <<'\n';
                    write[i] = first_val;
                    first_arr_p += 1;

                    if (first_arr_p == first_arr_ends_p) {
                        cout << "found the end of first array\n";
                        // copy rest of values from second arr to write
                        ++i;
                        while (i < arrs_end_p) {
                            write[i] = read[second_arr_p];
                            ++i;
                            ++second_arr_p;
                        }
                    }
                } else {
                    // cout << "second was smaller, writing " << second_val << " to index " << i <<'\n';
                    write[i] = second_val;
                    second_arr_p += 1;

                    if (second_arr_p == arrs_end_p) {
                        // cout << "found the end of the second arr\n";
                        // copy rest of values from first arr to write
                        ++i;
                        while (i < arrs_end_p) {
                            write[i] = read[first_arr_p];
                            ++i;
                            ++first_arr_p;
                        }
                    }
                }
            }
            cout << "after sorting: \n";
            for (int i = 0; i < n; ++i) {
                cout << write[i] <<' ';
            }
            cout << '\n';
            cout << '\n';
        }

        num_subarrays = num_subarrays / 2;
        merge_no += 1;
        if (num_subarrays == 1) {
            for (int i = 0; i < n; ++i) {
                data[i] = write[i];
            }
        }
    }


    // auto done2 = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done2-done).count();
}
