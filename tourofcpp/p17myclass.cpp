#include <iostream>

using namespace std;

class Vector {
    public:
        Vector(int s) :elem{new double[s]}, sz{s} {}
        double& operator[](int i){ return elem[i]; }
        int size() {return sz;}
    private:
        double* elem;
        int sz;
};

double read_and_sum(int s)
{
    Vector v(s);
    for(int i=0; i!=s; ++i) 
        cin >> v[i];
    
    double sum = 0;
    for (int i=0; i!=s; ++i)
        sum+=v[i];
    return sum;

}

int main()
{
    double sum = read_and_sum(3);
    cout << "Vektorin summa: " << sum << "\n";
}
