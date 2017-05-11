cmake_minimum_required(VERSION 3.7)

project(svg_converter)

include(cmake/dependencies/clipper.cmake)
include(cmake/dependencies/svgpp.cmake)
include(cmake/dependencies/boost.cmake)
include(cmake/dependencies/eigen3.cmake)
include(cmake/dependencies/libxml2.cmake)
include(cmake/dependencies/spdlog.cmake)

# Spdlog needs to be linked against pthreads on linux
find_package(Threads REQUIRED)

# Better to list these explicitly, see http://stackoverflow.com/q/1027247
set(CXX_SOURCE_FILES
        src/conversion.cpp
        src/logging.cpp
        src/main.cpp
        src/parsing/context/pattern.cpp
        src/parsing/context/transformable.cpp
        src/parsing/context/viewport_establishing.cpp
        src/parsing/coordinate_system.cpp
        src/parsing/gpgl_exporter.cpp
        src/parsing/path.cpp
        src/parsing/svgpp_external_parsers.cpp
        src/parsing/viewport.cpp
        src/svg.cpp)

set(CXX_SOURCE_AND_HEADER_FILES
        ${CXX_SOURCE_FILES}
        src/bezier.h
        src/conversion.h
        src/logging.h
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
        src/parsing/svgpp.h
        src/parsing/traversal.h
        src/parsing/viewport.h
        src/svg.h)

add_executable(${PROJECT_NAME} ${CXX_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(cmake/format.cmake)
    include(cmake/tidy.cmake)
    include(cmake/warnings.cmake)
endif()

target_link_libraries(${PROJECT_NAME}
        Svgpp
        Clipper
        Boost::boost
        Eigen3
        LibXml2
        spdlog::spdlog
        ${CMAKE_THREAD_LIBS_INIT})
