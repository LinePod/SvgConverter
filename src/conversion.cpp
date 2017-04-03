#include "conversion.h"

#include <sstream>

#include "parsing/context/g.h"
#include "parsing/context/pattern.h"
#include "parsing/context/shape.h"
#include "parsing/context/svg.h"
#include "parsing/gpgl_exporter.h"
#include "parsing/traversal.h"

std::string convert(const SvgDocument& svgDoc) {
    constexpr double print_area_width = 210;
    constexpr double print_area_height = 280;

    std::ostringstream code_stream;
    GpglExporter exporter{code_stream};
    const Viewport global_viewport{print_area_width, print_area_height};
    SvgContext<GpglExporter> context{svgDoc, exporter, global_viewport};
    xmlNodePtr root = svgDoc.root();
    DocumentTraversal::load_document(root, context);
    return code_stream.str();
}
