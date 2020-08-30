cmake_minimum_required(VERSION 3.16)

project(BearNet)

include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(BearNet)

option(BUILD_EXAMPLE "Build Example?" OFF)
if (BUILD_EXAMPLE)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
    link_libraries(BearNet)
    add_executable(bearnet example/main.cpp example/codec.cpp)
endif(BUILD_EXAMPLE)