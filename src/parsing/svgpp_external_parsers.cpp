// See svgpp.h for the reasons for this files existence

#include <svgpp/parser/external_function/parse_all_impl.hpp>

#include "traversal.h"



SVGPP_PARSE_PATH_DATA_IMPL(const char *, double)
SVGPP_PARSE_TRANSFORM_IMPL(const char *, double)
SVGPP_PARSE_PAINT_IMPL    (const char *, svgpp::factory::color::default_factory, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_COLOR_IMPL    (const char *, svgpp::factory::color::default_factory, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_PRESERVE_ASPECT_RATIO_IMPL(const char *)
SVGPP_PARSE_MISC_IMPL     (const char *, double)
SVGPP_PARSE_CLIP_IMPL     (const char *, LengthFactory)
SVGPP_PARSE_LENGTH_IMPL   (const char *, LengthFactory)