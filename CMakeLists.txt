cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(membench VERSION 1.0.0 LANGUAGES C)

add_executable(membench src/main.c src/print.c)

target_compile_options(membench PRIVATE -O3 -msse2 -mavx -Wall -Wextra -Wpedantic -Werror)