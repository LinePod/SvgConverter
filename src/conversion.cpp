#include "conversion.h"

#include <sstream>

#include "logging.h"
#include "parsing/context/g.h"
#include "parsing/context/pattern.h"
#include "parsing/context/shape.h"
#include "parsing/context/svg.h"
#include "parsing/gpgl_exporter.h"
#include "parsing/path.h"
#include "parsing/traversal.h"

std::string convert(const SvgDocument& svg_document) {
    constexpr double print_area_width = 210;
    constexpr double print_area_height = 280;

    std::ostringstream code_stream;
    spdlog::logger& logger = get_global_logger();
    GpglExporter exporter{code_stream};
    const Viewport global_viewport{print_area_width, print_area_height};
    SvgContext<GpglExporter> context{svg_document, logger, exporter, global_viewport};
    xmlNodePtr root = svg_document.root();

    try {
        DocumentTraversal::load_document(root, context);
    } catch (const InvalidPathError& err) {
        logger.critical("Invalid SVG: {}", err.what());
    }

    return code_stream.str();
}
