#include <iostream>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <libxml/parser.h>
#include <svgpp/policy/xml/libxml2.hpp>
#include <svgpp/svgpp.hpp>

#include "conversion.h"
#include "conversion/context/base.h"
#include "conversion/context/factories.h"

using processed_elements_t = boost::mpl::set<
        svgpp::tag::element::svg,
        svgpp::tag::element::g
//        svgpp::tag::element::circle,
//        svgpp::tag::element::ellipse,
//        svgpp::tag::element::line,
//        svgpp::tag::element::path,
//        svgpp::tag::element::polygon,
//        svgpp::tag::element::polyline,
//        svgpp::tag::element::rect
>::type;

using processed_attributes_t = boost::mpl::set<
        svgpp::tag::attribute::transform
>::type;

std::string convert(XmlDocument& svgDoc) {
    BaseContext context;
    svgpp::document_traversal<
            svgpp::processed_elements<processed_elements_t>,
            svgpp::processed_attributes<processed_attributes_t>,
            svgpp::context_factories<ContextFactories>
    >::load_document(svgDoc.root(), context);
    return {};
}
