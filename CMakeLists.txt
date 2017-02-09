cmake_minimum_required(VERSION 3.6)
project(svg_converter)

find_package(Boost REQUIRED)
find_package(LibXml2 REQUIRED)

set(SOURCE_FILES
        src/main.cpp
        src/xml.cpp
        src/conversion.cpp
        src/conversion/context/base.cpp
        src/conversion/context/shape.cpp)

add_executable(svg_converter ${SOURCE_FILES})
set_target_properties(svg_converter PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_compile_options(svg_converter
            PUBLIC -Weverything
            PUBLIC -Wno-c++98-compat)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(svg_converter
            PUBLIC -Wall
            PUBLIC -Wpedantic
            PUBLIC -Wextra)
endif()

# SYSTEM tells the compiler to treat the includes as system includes,
# suppressing the warnings in svg++.
target_include_directories(svg_converter SYSTEM
        PUBLIC ${Boost_INCLUDE_DIRS}
        PUBLIC ${LIBXML2_INCLUDE_DIR}
        PUBLIC external/svgpp/include)

target_link_libraries(svg_converter ${LIBXML2_LIBRARIES})
