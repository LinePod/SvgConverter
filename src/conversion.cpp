#include "conversion.h"

#include <sstream>

#include <svgpp/policy/xml/libxml2.hpp>
#include <svgpp/svgpp.hpp>

#include "parsing/context/factories.h"
#include "parsing/context/svg.h"
#include "parsing/gpgl_exporter.h"
#include "parsing/policies.h"

std::string convert(const ManagedXmlDoc& svgDoc) {
    constexpr double print_area_width = 210;
    constexpr double print_area_height = 280;

    std::ostringstream code_stream;
    GpglExporter exporter{code_stream};
    const Viewport global_viewport{print_area_width, print_area_height};
    SvgContext context{exporter, global_viewport};
    xmlNodePtr root = xmlDocGetRootElement(svgDoc.get());
    svgpp::document_traversal<
        svgpp::processed_elements<ProcessedElements>,
        svgpp::processed_attributes<ProcessedAttributes>,
        svgpp::context_factories<ChildContextFactories>,
        svgpp::path_policy<PathPolicy>, svgpp::length_policy<LengthPolicy>,
        svgpp::attribute_traversal<AttributeTraversalPolicy>,
        svgpp::viewport_policy<ViewportPolicy>>::load_document(root, context);
    return code_stream.str();
}
