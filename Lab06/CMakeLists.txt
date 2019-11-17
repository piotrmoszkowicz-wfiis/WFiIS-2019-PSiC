cmake_minimum_required(VERSION 3.14)
project(WFiIS_2019_PSiC_Lab06)

set(CMAKE_CXX_STANDARD 14)

include_directories(BEFORE /usr/local/opt/openssl/include)
find_library(OPENSSL_LIB ssl PATHS /usr/local/opt/openssl/lib NO_DEFAULT_PATH)
find_library(CRYPTO_LIB crypto PATHS /usr/local/opt/openssl/lib NO_DEFAULT_PATH)
link_directories(/usr/local/opt/openssl/lib)

add_executable(
        WFiIS_2019_PSiC_Lab06
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

target_link_libraries(WFiIS_2019_PSiC_Lab06
        crypto
        ssl
        )
