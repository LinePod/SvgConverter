# Add format target which formats sources using clang-format

foreach(REL_PATH ${CXX_SOURCE_AND_HEADER_FILES})
    get_filename_component(ABS_PATH ${REL_PATH} ABSOLUTE)
    list(APPEND CXX_ABSOLUTE_FORMAT_FILES ${ABS_PATH})
endforeach()

find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    add_custom_target(format
            COMMAND ${CLANG_FORMAT}
            -i
            -style=file
            ${CXX_ABSOLUTE_FORMAT_FILES})
else()
    message(WARNING "clang-format not found!")
endif()