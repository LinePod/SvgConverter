cmake_minimum_required(VERSION 3.5)

add_subdirectory(external)

project(svg_converter)

# Better to list these explicitly, see http://stackoverflow.com/q/1027247
set(CXX_SOURCE_FILES
        src/conversion.cpp
        src/main.cpp
        src/parsing/context/pattern.cpp
        src/parsing/context/transformable.cpp
        src/parsing/context/viewport_establishing.cpp
        src/parsing/coordinate_system.cpp
        src/parsing/gpgl_exporter.cpp
        src/parsing/path.cpp
        src/parsing/viewport.cpp
        src/svg.cpp)

set(CXX_SOURCE_AND_HEADER_FILES
        ${CXX_SOURCE_FILES}
        src/bezier.h
        src/conversion.h
        src/math_defs.h
        src/mpl_util.h
        src/parsing/context/base.h
        src/parsing/context/factories.h
        src/parsing/context/fwd.h
        src/parsing/context/g.h
        src/parsing/context/graphics_element.h
        src/parsing/context/pattern.h
        src/parsing/context/shape.h
        src/parsing/context/svg.h
        src/parsing/context/transformable.h
        src/parsing/context/viewport_establishing.h
        src/parsing/coordinate_system.h
        src/parsing/dashes.h
        src/parsing/gpgl_exporter.h
        src/parsing/path.h
        src/parsing/traversal.h
        src/parsing/viewport.h
        src/svg.h)

include(cmake/format.cmake)

find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(LibXml2 REQUIRED)


add_executable(${PROJECT_NAME} ${CXX_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_compile_options(${PROJECT_NAME}
            PUBLIC -Weverything
            PUBLIC -Wno-c++98-compat
            PUBLIC -Wno-padded
            PUBLIC -Wno-missing-prototypes)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME}
            PUBLIC -Wall
            PUBLIC -Wpedantic
            PUBLIC -Wextra)
endif()

# SYSTEM tells the compiler to treat the includes as system includes,
# suppressing the warnings in svg++.
target_include_directories(${PROJECT_NAME} SYSTEM
        PUBLIC ${Boost_INCLUDE_DIRS}
        PUBLIC ${Eigen3_INCLUDE_DIR}
        PUBLIC ${LIBXML2_INCLUDE_DIR}
        PUBLIC ${SVGPP_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES} Clipper)
