cmake_minimum_required(VERSION 3.6)
project(svg_converter)

find_package(Boost REQUIRED)
find_package(LibXml2 REQUIRED)

set(SOURCE_FILES
        main.cpp
        xml.cpp
        conversion.cpp
        conversion/context/base.cpp)

add_executable(svg_converter ${SOURCE_FILES})
set_target_properties(svg_converter PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# SYSTEM tells the compiler to treat the includes as system includes,
# suppressing the warnings in svg++.
target_include_directories(svg_converter SYSTEM
        PUBLIC ${Boost_INCLUDE_DIRS}
        PUBLIC ${LIBXML2_INCLUDE_DIR}
        PUBLIC external/svgpp/include)

target_link_libraries(svg_converter ${LIBXML2_LIBRARIES})
