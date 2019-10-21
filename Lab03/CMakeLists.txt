cmake_minimum_required(VERSION 3.14)
project(WFiIS_2019_PSiC_Lab03)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        WFiIS_2019_PSiC_Lab03
        main.cpp
        Utils/split_string.hpp
        Utils/split_string.cpp
        Utils/read_file.hpp
        Utils/read_file.cpp
        Utils/parse_headers.hpp
        Utils/parse_headers.cpp
        Utils/get_content_type.hpp
        Utils/get_content_type.cpp
        Utils/http_error_responses.cpp
        Utils/http_error_responses.hpp
        TCP/server.hpp
        TCP/server.cpp
        HTTP/handler.hpp
        HTTP/handler.cpp
        Utils/get_date.cpp Utils/get_date.hpp)