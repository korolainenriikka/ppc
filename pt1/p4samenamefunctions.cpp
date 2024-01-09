#include <iostream>

using namespace std;

void print(int i)
{
    std::cout << i << "\n";
}

void print(double dou)
{
    std::cout << dou << "\n";
}

void print(string str)
{
    std::cout << str << "\n";
}

int main()
{
    print(42);
    print("okei däääm aik hyvä");
    print(1.2354);
}
