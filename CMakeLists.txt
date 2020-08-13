cmake_minimum_required(VERSION 3.16)

project(BearNet)

include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 11)


add_subdirectory(BearNet)

link_libraries(BearNet)
add_executable(bearnet main.cpp)