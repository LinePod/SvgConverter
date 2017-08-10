#ifndef SVG_CONVERTER_PARSING_TRAVERSAL_H_
#define SVG_CONVERTER_PARSING_TRAVERSAL_H_

#include <type_traits>

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/vector.hpp>

#include "../mpl_util.h"
#include "context/factories.h"
#include "context/fwd.h"
#include "svgpp.h"
#include "viewport.h"

/**
 * Event that is fired after all viewport attributes have been parsed.
 *
 * We use our own event because we don't want the side effects defined for
 * svgpp::tag::event::after_viewport_attributes.
 */
struct AfterViewportAttributesEvent {};

namespace detail {

namespace mpl = boost::mpl;
namespace element = svgpp::tag::element;
namespace attrib = svgpp::tag::attribute;

/**
 * Controls SVG++'s traversal of the SVG document.
 *
 * Used to not traverse children of <svg> and <pattern> elements for which
 * rendering is disabled (viewport width and/or height of zero).
 */
struct DocumentTraversalControlPolicy {
    template <class Context>
    static bool proceed_to_element_content(const Context& context) {
        return context.process_children();
    }

    template <class Context>
    static bool proceed_to_next_child(const Context& /*unused*/) {
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

using PatternUnitAttributes =
    mpl::set2<mpl::pair<element::pattern, attrib::patternUnits>,
              mpl::pair<element::pattern, attrib::patternContentUnits>>;

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

    // Enable `stroke` only for shape elements
    PairAll<svgpp::traits::shape_elements, attrib::stroke>,

    // Enable transform attributes for all elements
    mpl::set<attrib::transform, attrib::patternTransform>,

    // Enable unit attributes for patterns
    PatternUnitAttributes>;

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
 * Make transforms from viewport into normal transforms, except for patterns.
 *
 * SVG++ contains code to translate viewport settings into a scaling plus a
 * translation, so we use that. Unfortunately, it does not handle patterns
 * correctly: SVG++ does not handle the attribute patternUnits, which changes
 * the interpretation of the x, y, width and height attributes. Normally, they
 * are interpreted as user units, but with patternUnits set to
 * objectBoundingBox, they are interpreted as factors relative to the bounding
 * box of the shape that should be filled with the pattern.
 * This wouldn't be a problem (because that can be adjusted for by simply
 * applying an additional scaling to the pattern), but patterns also support
 * the viewBox attribute, the contents of which are always in user units. When
 * the other attributes are bounding box factors and a viewbox is set, the
 * layout code produces a wrong layout, which cannot be fixed easily.
 * To avoid this, we calculate the viewport transformation ourselves for
 * patterns (reusing some of SVG++s code).
 */
struct ViewportPolicy {
    static const bool calculate_viewport = true;
    static const bool calculate_marker_viewport = true;
    static const bool calculate_pattern_viewport = false;
    static const bool viewport_as_transform = true;
};

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

/**
 * Controls attribute processing.
 *
 * We use the default settings but add some ordered processing of attributes
 * for <pattern>s, to enable our custom fixed viewport calculation (see
 * ViewportPolicy for why this is needed). We add an event after all viewport
 * attributes have been parsed, so that the viewport can be calculated. This is
 * needed to decide whether children elements should be parsed (compare
 * DocumentTraversalControlPolicy).
 */
struct AttributeTraversalPolicy
    : svgpp::policy::attribute_traversal::default_policy {
    struct get_priority_attributes_by_element {
        template <class ElementTag>
        using apply = mpl::if_<
            std::is_same<ElementTag, element::pattern>,
            typename mpl::vector<
                attrib::x, attrib::y, attrib::width, attrib::height,
                attrib::viewBox, attrib::preserveAspectRatio,
                attrib::patternUnits, attrib::patternContentUnits,
                svgpp::notify_context<AfterViewportAttributesEvent>>::type,
            mpl::empty_sequence>;
    };
};

}  // namespace detail

/**
 * SVG++ document traversal typedef with customized polices.
 */
using DocumentTraversal = svgpp::document_traversal<
    svgpp::processed_elements<detail::ProcessedElements>,
    svgpp::processed_attributes<detail::ProcessedAttributes>,
    svgpp::document_traversal_control_policy<
        detail::DocumentTraversalControlPolicy>,
    svgpp::attribute_traversal_policy<detail::AttributeTraversalPolicy>,
    svgpp::context_factories<ChildContextFactories>,
    svgpp::path_policy<detail::PathPolicy>,
    svgpp::length_policy<detail::LengthPolicy>,
    svgpp::viewport_policy<detail::ViewportPolicy>>;

#endif  // SVG_CONVERTER_PARSING_TRAVERSAL_H_
