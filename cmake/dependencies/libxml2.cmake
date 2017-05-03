find_package(LibXml2 REQUIRED)
if (LIBXML2_FOUND)
    add_library(LibXml2 INTERFACE IMPORTED)
    if (LIBXML2_INCLUDE_DIR)
        set_target_properties(LibXml2 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${LIBXML2_INCLUDE_DIR}")
    endif()
    if (LIBXML2_LIBRARIES)
        set_target_properties(LibXml2 PROPERTIES
                INTERFACE_LINK_LIBRARIES "${LIBXML2_LIBRARIES}")
    endif()
endif()
