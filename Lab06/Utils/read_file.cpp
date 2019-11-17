//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#include "read_file.hpp"

std::vector<char> Utils::read_file(const char *path) {
    std::vector<char> result;
    std::ifstream file (path, std::ios::in|std::ios::binary|std::ios::ate);
    file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    result.resize(file.tellg(), 0);
    file.seekg (0, std::ios::beg);
    file.read (result.data(), result.size());
    file.close();

    return result;
}