#include "Vector.h"
#include <stdexcept>

using namespace std;

template<typename T>
Vector<T>::Vector(int s) :elem{new T[s]}, sz{s}
{
    for (int i=0; i!=s; ++i)
        elem[i] = 0;
}

template<typename T>
T& Vector<T>::operator[](int i)
{
    if (i < 0 || size()<=i)
        throw out_of_range("Väärä indeksi senkin hölmö");
    return elem[i];
}

template<typename T>
Vector<T>::Vector(const Vector& a)
    :elem{new T[a.sz]},
    sz{a.sz}
{
    for (int i=0; i!=sz; ++i)
        elem[i] = a.elem[i];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& a)
{
    T* p = new T[a.sz];
    for (int i=0; i!=a.sz; ++i)
        p[i] = a.elem[i];
    delete[] elem;
    elem = p;
    sz = a.sz;
    return *this;
}

template<typename T>
int Vector<T>::size()
{
    return sz;
}

template class Vector<double>;