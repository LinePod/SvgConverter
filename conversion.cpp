#include <iostream>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <libxml/parser.h>
#include <svgpp/policy/xml/libxml2.hpp>
#include <svgpp/svgpp.hpp>

#include "conversion.h"
#include "conversion/context/base.h"
#include "conversion/context/factories.h"

/**
 * List of elements which should be processed.
 */
using processed_elements_t = boost::mpl::set<
        svgpp::tag::element::svg,
        svgpp::tag::element::g,
        svgpp::tag::element::circle,
        svgpp::tag::element::ellipse,
        svgpp::tag::element::line,
        svgpp::tag::element::path,
        svgpp::tag::element::polygon,
        svgpp::tag::element::polyline,
        svgpp::tag::element::rect
>::type;

/**
 * List of attributes which should be processed.
 */
using processed_attributes_t = boost::mpl::insert<
        svgpp::traits::shapes_attributes_by_element,
        svgpp::tag::attribute::transform
>::type;

/**
 * Policy on how to handle paths (and other elements converted to paths).
 *
 * The `minimal` policy does all the conversions described at
 * http://svgpp.org/path.html#path-policy-concept. The conversion from arcs
 * to bézier curves is lossy. Because the silhouette can only plot arcs of
 * circles but not arcs a conversion would be necessary anyway. Can be
 * replaced with another conversion if artifacts start to appear.
 */
using path_policy_t = svgpp::policy::path::minimal;

std::string convert(XmlDocument& svgDoc) {
    BaseContext context;
    svgpp::document_traversal<
            svgpp::processed_elements<processed_elements_t>,
            svgpp::processed_attributes<processed_attributes_t>,
            svgpp::context_factories<ContextFactories>,
            svgpp::path_policy<path_policy_t>
    >::load_document(svgDoc.root(), context);
    return {};
}
