#include <iostream>

using namespace std;

int main()
{
    std::cout << sizeof(int) << "\n";
    std::cout << sizeof(bool) << "\n";
    std::cout << sizeof(char) << "\n";
    std::cout << sizeof(double) << "\n";
    std::cout << sizeof(unsigned) << "\n";

    int a = -1;
    a = -a; // changes the sign
    std::cout << a << "\n";

    // test truncating double to int
    double b = 2.2;
    int i = 7;
    i = b*i;
    // int j {b * 7}; curly bracket assignment outputs warning
    // outputs 15: 2.2*7=15.4, decimal is removed to get int
    std::cout << i << "\n";
}