//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#include <iostream>
#include "split_string.hpp"

std::vector<std::string> Utils::split_string(std::string &str_to_split, const std::string delimiter) { //NOLINT
    size_t last = 0;
    size_t delimiter_length = delimiter.size();
    size_t next = 0;
    std::vector<std::string> result;

    while ((next = str_to_split.find(delimiter, last)) != std::string::npos) {
        result.push_back(str_to_split.substr(last, next - last));
        last = next + delimiter_length;
    }

    result.push_back(str_to_split.substr(last));

    return result;
}