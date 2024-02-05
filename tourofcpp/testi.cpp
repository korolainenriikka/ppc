
#include <iostream>

typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));

int main()
{
    crash()
    float8_t a,b,c;
    for (int i = 0; i<8; ++i){
        a[i] = i;
        b[i] = i;
    }

    c = a+b;
}