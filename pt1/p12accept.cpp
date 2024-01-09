#include <iostream>

using namespace std;

int main()
{
    bool proceeded = true;
    while(proceeded){
        cout << "Proceed? (y/n)\n";

        char answer = 0;
        cin >> answer;

        if (answer == 'n')
            proceeded = false;
    }
    
}