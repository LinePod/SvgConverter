#include "conversion.h"

#include <sstream>

#include <svgpp/policy/xml/libxml2.hpp>

#include "parsing/context/svg.h"
#include "parsing/gpgl_exporter.h"
#include "parsing/traversal.h"

std::string convert(const SvgDocument& svgDoc) {
    constexpr double print_area_width = 210;
    constexpr double print_area_height = 280;

    std::ostringstream code_stream;
    GpglExporter exporter{code_stream};
    const Viewport global_viewport{print_area_width, print_area_height};
    SvgContext<GpglExporter> context{exporter, global_viewport};
    xmlNodePtr root = svgDoc.root();
    DocumentTraversal::load_document(root, context);
    return code_stream.str();
}
