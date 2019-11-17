cmake_minimum_required(VERSION 3.14)
project(WFiIS_2019_PSiC_Server)

set(CMAKE_CXX_STANDARD 14)

include_directories(BEFORE /usr/local/opt/openssl/include)
find_library(OPENSSL_LIB ssl PATHS /usr/local/opt/openssl/lib NO_DEFAULT_PATH)
find_library(CRYPTO_LIB crypto PATHS /usr/local/opt/openssl/lib NO_DEFAULT_PATH)
link_directories(/usr/local/opt/openssl/lib)

add_executable(WFiIS_2019_PSiC_Server main.cpp server.hpp server.cpp send_unreliable.h)

target_link_libraries(WFiIS_2019_PSiC_Server
        crypto
        )
