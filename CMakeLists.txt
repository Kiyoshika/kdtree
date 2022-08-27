cmake_minimum_required(VERSION 3.10.0)
project(kdtree)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -O0 -g")

add_subdirectory(ext/CMatrix)

add_subdirectory(src)