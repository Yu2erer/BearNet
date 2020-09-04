cmake_minimum_required(VERSION 3.16)

project(BearNet)

include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(BearNet)
add_definitions("-O0 -Wall -g -ggdb")

link_directories(
    "./lib"
)

option(BUILD_EXAMPLE "Build Example?" ON)
if (BUILD_EXAMPLE)
    include_directories(protobuf/include)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
    link_libraries(protobuf)
    link_libraries(bearnet)
    add_executable(bearServer example/main.cpp example/example.pb.cc)
endif(BUILD_EXAMPLE)