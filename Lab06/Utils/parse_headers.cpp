//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#include "parse_headers.hpp"

std::string Utils::create_header_string(const char* header, const char* value) {
    return std::string(header) + ": " + value + "\n";
}

std::vector<std::pair<std::string, std::string>> Utils::parse_request_details_to_headers_vector(
        std::vector<std::string> headers) {
    auto result = std::vector<std::pair<std::string, std::string>>{};

    headers.erase(headers.begin());

    for (auto& header : headers) {
        auto header_parsed = Utils::split_string(header, ": ");
        result.emplace_back(
                header_parsed[0],
                header_parsed[1]
        );
    }

    return result;
}