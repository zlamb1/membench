cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(membench VERSION 1.0.0 LANGUAGES C)

function(create_target target_name)
    add_executable(${target_name} ${ARGN})
    target_include_directories(${target_name} PRIVATE src)
    target_compile_options(${target_name} PRIVATE -O3 -msse2 -mavx -Wall -Wextra -Wpedantic -Werror)
endfunction()

create_target(membench src/main.c src/print.c)

create_target(memset32 src/test/memset32_bench.c src/print.c)