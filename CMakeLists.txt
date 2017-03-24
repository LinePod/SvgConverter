cmake_minimum_required(VERSION 3.6)
project(svg_converter)

add_subdirectory(external)

find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(LibXml2 REQUIRED)

set(SOURCE_FILES
        src/main.cpp
        src/svg.cpp
        src/conversion.cpp
        src/parsing/viewport.cpp
        src/parsing/coordinate_system.cpp
        src/parsing/gpgl_exporter.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
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
