//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#pragma once

#include <cstring>

namespace Utils {
    /**
     * @fn Utils::get_content_type
     * @param [const char*]     extension           - Extension of file
     * @return [const char*]                        - Mime-Type of the file for the HTTP request
     */
    const char *get_content_type(const char *extension);
}
