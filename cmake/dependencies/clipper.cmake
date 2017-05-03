add_library(Clipper STATIC external/clipper/clipper.cpp)

# SYSTEM to suppress warnings from clippers code
target_include_directories(Clipper SYSTEM PUBLIC external/clipper/)
