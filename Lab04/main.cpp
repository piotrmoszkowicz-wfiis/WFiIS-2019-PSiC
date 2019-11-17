//
// Created by Piotr Moszkowicz on 23/10/2019.
//

#include <iostream>

int main() {
    int x = 1;

    auto y = new int(2);

    std::cout << (void*)(&x) << std::endl;
    std::cout << (void*)(y) << std::endl;
    std::cout << (void*)main << std::endl;

    delete y;

    return 0;
}