cmake_minimum_required(VERSION 2.8.10)

project(Clipper)

add_library(${PROJECT_NAME} STATIC clipper.cpp)

# SYSTEM to suppress warnings from clippers code
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC .)
