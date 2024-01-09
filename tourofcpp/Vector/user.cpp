#include "Vector.h"
#include <cmath>
#include <iostream>

using namespace std;

double sqrt_sum(Vector<double>& v)
{
    double sum = 0;
    for (int i=0; i!=v.size(); ++i)
        sum += sqrt(v[i]);
    return sum;
}

int main()
{
    Vector<double> v(3);
    v[0] = 1.1;
    v[1] = 2.2;
    v[2] = 3.3;
    try
    {
        v[3] = 4.4;
    }
    catch(const std::out_of_range& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    cout << sqrt_sum(v) << "\n";

    Vector<double> v2 = v;
}

