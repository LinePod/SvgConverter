cmake_minimum_required(VERSION 3.6)
project(svg_converter)

set(SOURCE_FILES main.cpp)

add_executable(svg_converter ${SOURCE_FILES})
set_target_properties(svg_converter PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
