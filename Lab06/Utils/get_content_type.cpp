//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#include "get_content_type.hpp"

const char * Utils::get_content_type(const char* extension) {
    if (strcmp(extension, "html") == 0) {
        return "text/html";
    } else if (strcmp(extension, "jpg") == 0) {
        return "image/jpeg";
    } else if (strcmp(extension, "js") == 0) {
        return "application/javascript";
    } else {
        return "application/octet-stream";
    }
}