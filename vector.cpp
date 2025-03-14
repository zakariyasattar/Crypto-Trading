#include <stdlib.h>
#include <iostream>

template <typename T>
class Vector {
private:
    size_t mCapacity;
    size_t mSize;
    T* ptr;

public:
    Vector() {
        mCapacity = 0;
        mSize = 0;
        ptr = nullptr;
    }

    Vector(size_t size, T val) : mSize (size), mCapacity (size) {
        T* arr = new T[mSize];

        for(int i = 0; i < mSize; i++) {
            arr[i] = val;
        }

        ptr = arr;
    }

    Vector(size_t size) : mSize (size), mCapacity (size) {
        T* arr = new T[size];
        ptr = arr;
    }

    ~Vector() { delete[] ptr; }

    size_t size() const { return mSize; }

    const T& operator[](int pos) {
        if(pos > mSize) throw std::runtime_error("Out of Bounds");
        return ptr[pos];
    }

    void push_back(T val) {
        if(mSize == mCapacity) {
            resize();
        }

        ptr[mSize] = val;
        mSize++;
    }

    void resize() {
        if(mCapacity == 0) mCapacity = 1;
        else mCapacity *= 2;

        T* newPtr = new T[mCapacity];

        for(int i = 0; i < mSize; i++) {
            newPtr[i] = ptr[i];
        }

        delete[] ptr;
        ptr = newPtr;
    }
};

using namespace std;

int main() {
    Vector<int> v (5, 4);
    std::cout << v[3] << std::endl;

    v.push_back(7);

    cout << v.size() << " " << v[3] << endl;

    return 0;
}