#ifndef SVG_CONVERTER_PARSING_TRAVERSAL_H_
#define SVG_CONVERTER_PARSING_TRAVERSAL_H_

#include <boost/mpl/map.hpp>
#include <svgpp/document_traversal.hpp>
#include <svgpp/policy/path.hpp>
#include <svgpp/policy/viewport.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/element_groups.hpp>

#include "../mpl_util.h"
#include "context/base.h"
#include "context/factories.h"
#include "context/g.h"
#include "context/shape.h"
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

    template <class Exporter>
    static bool proceed_to_element_content(
        const SvgContext<Exporter>& context) {
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

    PairAll<svgpp::traits::shape_elements, attrib::fill>,

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
 *
 * This is basically a version of `svgpp::policy::length::forward_to_method`
 * that doesn't need a common base type for the contexts.
 */
struct LengthPolicy {
    using length_factory_type = const LengthFactory;

    template <class Context>
    static length_factory_type& length_factory(Context& context) {
        return context.length_factory();
    }
};

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

/**
 * SVG++ document traversal typedef with customized polices.
 */
using DocumentTraversal = svgpp::document_traversal<
    svgpp::processed_elements<ProcessedElements>,
    svgpp::processed_attributes<ProcessedAttributes>,
    svgpp::context_factories<ChildContextFactories>,
    svgpp::path_policy<PathPolicy>, svgpp::length_policy<LengthPolicy>,
    svgpp::attribute_traversal<AttributeTraversalPolicy>,
    svgpp::viewport_policy<ViewportPolicy>>;

}  // namespace detail

using detail::DocumentTraversal;

#endif  // SVG_CONVERTER_PARSING_TRAVERSAL_H_
