#ifndef SVG_CONVERTER_PARSING_TRAVERSAL_H_
#define SVG_CONVERTER_PARSING_TRAVERSAL_H_

#include <svgpp/policy/xml/libxml2.hpp>

#include <boost/mpl/map.hpp>
#include <svgpp/document_traversal.hpp>
#include <svgpp/policy/path.hpp>
#include <svgpp/policy/viewport.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/element_groups.hpp>

#include "../mpl_util.h"
#include "context/factories.h"
#include "context/fwd.h"
#include "context/viewport_establishing.h"
#include "viewport.h"

namespace detail {

namespace mpl = boost::mpl;
namespace element = svgpp::tag::element;
namespace attrib = svgpp::tag::attribute;

/**
 * Controls SVG++'s traversal of the SVG document.
 *
 * Used to not traverse children of <svg> or <pattern> elements for which
 * rendering is disabled (viewport width and/or height of zero).
 */
struct DocumentTraversalControlPolicy {
    static bool proceed_to_element_content(const BaseContext&) { return true; }

    static bool proceed_to_element_content(
        const ViewportEstablishingContext& context) {
        return !context.rendering_disabled();
    }

    template <class Context>
    static bool proceed_to_next_child(const Context&) {
        return true;
    }
};

/**
 * List of elements which should be processed.
 */
using ProcessedElements = Concat<
    // Elements describing shapes
    svgpp::traits::shape_elements,

    // Supported structural elements
    mpl::set<element::svg, element::g>>;

/**
 * List of attributes which should be processed.
 */
using ProcessedAttributes = Concat<
    // Attributes describing the shape of shape elements
    svgpp::traits::shapes_attributes_by_element,

    // Viewport attributes for `<svg>` and `<pattern>`
    svgpp::traits::viewport_attributes,

    // Enable `stroke-dasharray` only for shape elements
    PairAll<svgpp::traits::shape_elements, attrib::stroke_dasharray>,

    // Enable `fill` only for shape elements
    PairAll<svgpp::traits::shape_elements, attrib::fill>,

    // Enable transform attributes for all elements
    mpl::set<attrib::transform, attrib::patternTransform>>;

/**
 * Policy on how to handle paths (and other elements converted to paths).
 *
 * The `minimal` policy does all the conversions described at
 * http://svgpp.org/path.html#path-policy-concept. The conversion from arcs
 * to bézier curves is lossy. Because the silhouette can only plot arcs of
 * circles but not ellipses a conversion would be necessary anyway. Can be
 * replaced with another conversion if artifacts start to appear.
 */
using PathPolicy = svgpp::policy::path::minimal;

/**
 * Handle transformations from viewport settings like normal transformations.
 *
 * Allows code reuse, because the reason for the transformation (viewport
 * attributes or transform attribute) is not important for us.
 */
using ViewportPolicy = svgpp::policy::viewport::as_transform;

/**
 * Obtain the length factory for length conversions from the context instance.
 *
 * The context must provide a method `length_factory` which returns a reference
 * to the length factory.
 *
 * This is basically a version of `svgpp::policy::length::forward_to_method`
 * that doesn't need a common base type for the contexts which defines
 * `length_factory`.
 */
struct LengthPolicy {
    using length_factory_type = const LengthFactory;

    template <class Context>
    static length_factory_type& length_factory(Context& context) {
        return context.length_factory();
    }
};

}  // namespace detail

/**
 * SVG++ document traversal typedef with customized polices.
 */
using DocumentTraversal = svgpp::document_traversal<
    svgpp::processed_elements<detail::ProcessedElements>,
    svgpp::processed_attributes<detail::ProcessedAttributes>,
    svgpp::context_factories<ChildContextFactories>,
    svgpp::path_policy<detail::PathPolicy>,
    svgpp::length_policy<detail::LengthPolicy>,
    svgpp::viewport_policy<detail::ViewportPolicy>>;

#endif  // SVG_CONVERTER_PARSING_TRAVERSAL_H_
