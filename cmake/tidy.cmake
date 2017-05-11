# Add clang-tidy integration

find_program(CLANG_TIDY "clang-tidy")
if (CLANG_TIDY)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY}")
else()
    message(WARNING "clang-tidy not found!")
endif()
