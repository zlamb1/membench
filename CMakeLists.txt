cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(membench VERSION 1.0.0 LANGUAGES C)

function(create_target target_name)
    add_executable(${target_name} src/print.c ${ARGN})
    target_include_directories(${target_name} PRIVATE src)
    target_compile_options(${target_name} PRIVATE -O2 -msse2 -mavx -Wall -Wextra -Wpedantic -Werror)
endfunction()

create_target(membench src/main.c)
create_target(memset src/test/memset_bench.c)
create_target(memset32 src/test/memset32_bench.c)