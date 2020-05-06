cmake_minimum_required(VERSION 3.16)
project(myvk C)

set(CMAKE_C_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(myvk myvk.c myvk.h)

add_subdirectory(example)