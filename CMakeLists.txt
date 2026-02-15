cmake_minimum_required(VERSION 3.16)
project(tlac C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable POSIX APIs declarations (getaddrinfo, ftruncate, nanosleep, etc.)
add_compile_definitions(_POSIX_C_SOURCE=200809L)
add_compile_definitions(_DEFAULT_SOURCE)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Reasonable warnings
if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

add_subdirectory(src/clock)
add_subdirectory(src/helper)
add_subdirectory(src/trace)
add_subdirectory(src/usage)
add_subdirectory(example)
