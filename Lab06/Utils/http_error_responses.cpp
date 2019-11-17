//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#include "http_error_responses.hpp"

const char* Utils::not_found_response() {
    return "<html>\n"
           "<head>\n"
           "<title>Object not found</title>\n"
           "</head>\n"
           "<body>\n"
           "<h1>404 - not found</h1>\n"
           "</body>\n"
           "</html>";
}