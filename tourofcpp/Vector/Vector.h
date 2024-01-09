template<typename T>
class Vector {
    public:
        Vector(int s);
        ~Vector() { delete[] elem; };

        Vector(const Vector& a);
        Vector& operator=(const Vector& a);

        T& operator[](int i);

        int size();
    private:
        T* elem;
        int sz;
};
