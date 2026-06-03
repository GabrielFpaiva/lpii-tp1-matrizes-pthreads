cmake_minimum_required(VERSION 3.10)
project(matmul C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Garante as flags exigidas mesmo sem build type definido.
add_compile_options(-O2 -Wall -Wextra)

find_package(Threads REQUIRED)

file(GLOB SOURCES src/*.c)
add_executable(matmul ${SOURCES})
target_link_libraries(matmul PRIVATE Threads::Threads)
