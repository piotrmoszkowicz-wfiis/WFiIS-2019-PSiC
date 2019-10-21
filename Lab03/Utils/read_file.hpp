//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#pragma once

#include <fstream>
#include <vector>

namespace Utils {
    /**
     * @fn read_file
     * @brief Reads file into the vector of chars
     * @param [const char*]         path            -   Path to the file to read
     * @return [std::vector<char>]                  -   Vector of file's content
     */
    std::vector<char> read_file(const char *path);
};

