#ifndef SVG_CONVERTER_POLICIES_H
#define SVG_CONVERTER_POLICIES_H

#include <svgpp/policy/path.hpp>
#include <svgpp/policy/viewport.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/element_groups.hpp>

#include "../utility.h"
#include "context/svg.h"
#include "viewport.h"

/**
 * Controls SVG++'s traversal of the SVG document.
 *
 * Used to not traverse children of <svg> elements for which rendering is
 * disabled (viewport width and/or height of zero).
 */
struct DocumentTraversalControlPolicy {
    template <class Context>
    static bool proceed_to_element_content(const Context&) {
        return true;
    }

    static bool proceed_to_element_content(const SvgContext& context) {
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
    boost::mpl::set<svgpp::tag::element::svg, svgpp::tag::element::g>>;

/**
 * List of attributes which should be processed.
 */
using ProcessedAttributes = Concat<
    // Attributes describing the shape of shape elements
    svgpp::traits::shapes_attributes_by_element,

    // Viewport attributes for `<svg>` and `<pattern>`
    svgpp::traits::viewport_attributes,

    // Other attributes
    boost::mpl::set<  // NOLINT not the stl set (no #include <set> needed)
        svgpp::tag::attribute::transform>>;

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
 */
using LengthPolicy =
    svgpp::policy::length::forward_to_method<GraphicsElementContext,
                                             const LengthFactory>;

#endif  // SVG_CONVERTER_POLICIES_H
