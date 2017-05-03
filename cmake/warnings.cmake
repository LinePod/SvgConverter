# Enable lots of warnings (on Clang and GCC)

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
