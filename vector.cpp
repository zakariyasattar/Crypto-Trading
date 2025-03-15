#include <stdlib.h>
#include <iostream>

// template <typename T>
// class Vector {
// private:
//     size_t mCapacity;
//     size_t mSize;
//     std::vector<std::unique_ptr<T>> mNodes;
//     T* ptr;

// public:
//     Vector() {
//         mCapacity = 0;
//         mSize = 0;
//         ptr = nullptr;
//     }

//     Vector(size_t size, T val) : mSize (size), mCapacity (size) {
//         T* arr = new T[mSize];

//         for(int i = 0; i < mSize; i++) {
//             arr[i] = val;
//         }

//         ptr = arr;
//     }

//     Vector(size_t size) : mSize (size), mCapacity (size) {
//         mNodes.reserve(size);
//         ptr = mNodes[0];
//     }

//     ~Vector() { delete[] ptr; }

//     size_t size() const { return mSize; }

//     const T& operator[](int pos) {
//         if(pos > mSize) throw std::runtime_error("Out of Bounds");
//         return *(mNodes[pos]);
//     }

//     void push_back(T val) {

//         mNodes.push_back(std::make_unique<T>(val));
//         mSize++;
//     }

//     void resize() {
//         if(mCapacity == 0) mCapacity = 1;
//         else mCapacity *= 2;

//         T* newPtr = new T[mCapacity];

//         for(int i = 0; i < mSize; i++) {
//             newPtr[i] = ptr[i];
//         }

//         delete[] ptr;
//         ptr = newPtr;
//     }

//     T* begin() {
//         return ptr;
//     }

//     T* end() {
//         return ptr + mSize;
//     }
// };

using namespace std;

int main() {
    // Vector<int> v (5, 4);



    // int* it = v.begin();

    // cout << *it << endl;

    // v.push_back(5);

    // cout << *it << endl;

    cout << __cplusplus << endl;

    return 0;
}