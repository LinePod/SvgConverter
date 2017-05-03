get_filename_component(SVGPP_INCLUDE_DIR external/svgpp/include ABSOLUTE)
if (SVGPP_INCLUDE_DIR)
    add_library(Svgpp INTERFACE IMPORTED)
    set_target_properties(Svgpp PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SVGPP_INCLUDE_DIR}")
endif()
