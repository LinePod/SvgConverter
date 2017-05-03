find_package(Eigen3 REQUIRED)
if (EIGEN3_FOUND)
    add_library(Eigen3 INTERFACE IMPORTED)
    if (EIGEN3_INCLUDE_DIR)
        set_target_properties(Eigen3 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}")
    endif()
endif()
