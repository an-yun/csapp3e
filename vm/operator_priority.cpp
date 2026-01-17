//
// Created by zuo on 2026/1/17.
//

#include <iostream>

void print(const int* arr, size_t size) {
    using std::cout;
    cout << "the address of arr is" << arr << std::endl;
    cout << "the data is" << std::endl;
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << std::endl;
}

int main() {
    size_t size = 4;
    int* original_arr = new int[size]{};
    int* arr = original_arr;
    print(arr, size);
    /**
     * 右运算从右往左
     **/
    *arr++;
    print(arr, size);
    (*arr)++;
    print(arr, size);
    delete[] original_arr;
    return 0;
}