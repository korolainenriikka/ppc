
#include <iostream>
#include <vector>

using namespace std;

void increment(vector<int>& v)
{
    ++v[8];
}

int main()
{
    vector<int> v1 = {0,1,2,3,4,5,6,7,8,9};
    vector<int> v2;

    for(auto &i: v1)
    {
        v2.push_back(i);
    }

    std::cout << v2[8] << "\n";
    increment(v2);
    std::cout << v2[8] << "\n";

}