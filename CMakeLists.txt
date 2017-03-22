cmake_minimum_required(VERSION 3.6)
project(svg_converter)

add_subdirectory(external)

find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(LibXml2 REQUIRED)

set(SOURCE_FILES
        src/main.cpp
        src/xml.cpp
        src/conversion.cpp
        src/parsing/viewport.cpp
        src/parsing/coordinate_system.cpp
        src/parsing/context/graphics_element.cpp
        src/parsing/context/svg.cpp
        src/parsing/context/g.cpp
        src/parsing/context/shape.cpp
        src/parsing/gpgl_exporter.cpp)

add_executable(svg_converter ${SOURCE_FILES})
set_target_properties(svg_converter PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_compile_options(svg_converter
            PUBLIC -Weverything
            PUBLIC -Wno-c++98-compat
            PUBLIC -Wno-padded
            PUBLIC -Wno-missing-prototypes)
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
        PUBLIC ${Eigen3_INCLUDE_DIR}
        PUBLIC ${LIBXML2_INCLUDE_DIR}
        PUBLIC ${SVGPP_INCLUDE_DIR})

target_link_libraries(svg_converter ${LIBXML2_LIBRARIES})
