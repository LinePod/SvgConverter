#ifndef SVG_CONVERTER_PARSING_SVGPP_H
#define SVG_CONVERTER_PARSING_SVGPP_H

// This file should be the only place that svgpp headers are included from,
// to assure that the following defines are set before any headers are included.

// Makes some SVG++ calls virtual, but improves compilation speed
// http://svgpp.org/faq.html#compiler-is-out-of-memory-or-compilation-takes-too-long
#define SVGPP_USE_EXTERNAL_PATH_DATA_PARSER
#define SVGPP_USE_EXTERNAL_TRANSFORM_PARSER
#define SVGPP_USE_EXTERNAL_PRESERVE_ASPECT_RATIO_PARSER
#define SVGPP_USE_EXTERNAL_PAINT_PARSER
#define SVGPP_USE_EXTERNAL_MISC_PARSER
#define SVGPP_USE_EXTERNAL_COLOR_PARSER
#define SVGPP_USE_EXTERNAL_LENGTH_PARSER

// Allow for more named template arguments to be passed to SVG++'s
// document_traversal class. Should ideally be defined before any boost import
// in any cpp file, but redefining it here works as well.
#define BOOST_PARAMETER_MAX_ARITY 10

#include <libxml/parser.h>
#include <svgpp/policy/xml/libxml2.hpp>
#include <svgpp/svgpp.hpp>

#endif  // SVG_CONVERTER_PARSING_SVGPP_H
