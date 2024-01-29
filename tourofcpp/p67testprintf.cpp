#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

string compose(string str1, string str2)
{
    return str1 + " " + str2;
}

int main()
{
    printf("moi %s %s", "okei", "toimiiko");
    printf("tää vissiin toimii\n");
    string yhdistetty = compose("yhdistä", "nämä");
    cout << yhdistetty;

}