//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace Utils {
    /**
     * @fn split_string
     * @brief Splits string by string delimiter
     * @param [std::string&]    str_to_split        - String, which will be splitted
     * @param [std::string]     delimiter           - String, which will be used as delimiter
     * @return [std::vector<std::string>]           - Vector of splitted strings
     */
    std::vector<std::string> split_string(std::string &str_to_split, const std::string delimiter); //NOLINT
}
