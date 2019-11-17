//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#pragma once

#include <string>
#include <vector>

#include "split_string.hpp"

namespace Utils {
    /**
     * @fn parse_request_details_to_headers_vector
     * @brief Parses request vector of header's strings' into the vector Pair<Header, Value>
     * @param [std::vector<std::string> headers                     -   Vector of headers strings'
     * @return [std::vector<std::pair<std::string, std::string>>]   -   Vector of Pair<Header, Value>
     */
    std::vector<std::pair<std::string, std::string>>
    parse_request_details_to_headers_vector(std::vector<std::string> headers);

    /**
     * @fn create_header_string
     * @brief Creates single header string from header and value
     * @param [const char*]             header                      -   Header name
     * @param [const char*]             value                       -   Header value
     * @return [std::string]                                        -   Formatted header
     */
    std::string create_header_string(const char *header, const char *value);
}
