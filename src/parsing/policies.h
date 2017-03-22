#ifndef SVG_CONVERTER_POLICIES_H
#define SVG_CONVERTER_POLICIES_H

#include <boost/mpl/map.hpp>
#include <svgpp/policy/path.hpp>
#include <svgpp/policy/viewport.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/element_groups.hpp>

#include "../mpl_util.h"
#include "context/svg.h"
#include "viewport.h"

namespace detail {

namespace mpl = boost::mpl;
namespace element = svgpp::tag::element;
namespace attrib = svgpp::tag::attribute;

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

    // Other attributes for all elements
    mpl::set<attrib::transform>>;

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

/**
 * Defines order and priority of attributes being parsed.
 *
 * We use this to defer parsing of the points attributes for `<path>`,
 * `<polygon>` and `<polyline>` so that all presentation attributes are parsed
 * before reaching the points. This allows us to immediately emit code for those
 * shapes. The adapters for `<rect>`, `<circle>` and the like already wait until
 * all attributes are processed to emit path events, so no change is needed for
 * those.
 */
struct AttributeTraversalPolicy
    : svgpp::policy::attribute_traversal::default_policy {
 private:
    /**
     * MPL map specifying the deferred attributes per element.
     *
     * By default, no attributes are deferred.
     */
    using DeferredElements = typename mpl::map<
        mpl::pair<element::path, mpl::set<attrib::d>>,
        mpl::pair<element::polyline, mpl::set<attrib::points>>,
        mpl::pair<element::polygon, mpl::set<attrib::points>>>::type;

 public:
    struct get_deferred_attributes_by_element {
        template <class ElementTag>
        struct apply {
            using type =
                AtOrDefault<DeferredElements, ElementTag, mpl::empty_sequence>;
        };
    };
};

}  // namespace detail

using detail::DocumentTraversalControlPolicy;
using detail::ProcessedElements;
using detail::ProcessedAttributes;
using detail::PathPolicy;
using detail::ViewportPolicy;
using detail::LengthPolicy;
using detail::AttributeTraversalPolicy;

#endif  // SVG_CONVERTER_POLICIES_H
