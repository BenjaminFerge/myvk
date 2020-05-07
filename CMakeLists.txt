cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(myvk C)

set(CMAKE_C_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(myvk myvk.c myvk.h)

add_subdirectory(example)

find_package(Vulkan REQUIRED)
target_include_directories(myvk PRIVATE Vulkan::Vulkan)
target_link_libraries(myvk PRIVATE ${CONAN_LIBS} Vulkan::Vulkan)
