cmake_minimum_required(VERSION 3.16)

project(BearNet)

include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 11)

link_directories(LIBRARY_OUTPUT_PATH)

add_subdirectory(BearNet)
add_definitions("-O0 -Wall -g -ggdb")

option(BUILD_EXAMPLE "Build Example?" ON)
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif(BUILD_EXAMPLE)