#ifndef SVG_CONVERTER_PARSING_CONTEXT_FACTORIES_H_
#define SVG_CONVERTER_PARSING_CONTEXT_FACTORIES_H_

#include <type_traits>
#include <utility>

#include <boost/mpl/has_key.hpp>

#include "../svgpp.h"
#include "fwd.h"

namespace detail {

namespace element = svgpp::tag::element;

/**
 * Constant specifying whether the given element is a shape element.
 */
template <class ElementTag>
constexpr bool kIsShapeElement =
    boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag>::value;

/**
 * Derives the type of the inner exporter from a parent context type.
 */
template <class ParentContext>
using InnerExporter = decltype(std::declval<ParentContext>().inner_exporter());

/**
 * Definitions for `ChildContextFactories`.
 *
 * All types will be wrapped `svgpp::factory::context::on_stack<>` in
 * `ChildContextFactories` (not done here to reduce boilerplate).
 */
template <class ParentContext, class ElementTag, class Enable = void>
struct CCFImpl {};

template <class ParentContext>
struct CCFImpl<ParentContext, element::svg> {
    using type = SvgContext<InnerExporter<ParentContext>>;
};

template <class ParentContext>
struct CCFImpl<ParentContext, element::g> {
    using type = GContext<InnerExporter<ParentContext>>;
};

template <class ParentContext, class ElementTag>
struct CCFImpl<ParentContext, ElementTag,
               std::enable_if_t<kIsShapeElement<ElementTag>>> {
    using type = ShapeContext<InnerExporter<ParentContext>>;
};

template <class Exporter>
struct CCFImpl<ShapeContext<Exporter>, element::pattern> {
    using type = PatternContext<InnerExporter<ShapeContext<Exporter>>>;
};

}  // namespace detail

/**
 * Describes how to create context instances for each SVG element.
 *
 * We use `svgpp::factory::context::on_stack<>` for everything, meaning that
 * a new instance is created for each element, and that the instances will get
 * a reference to the parent context passed to their constructor.
 *
 * All graphics element contexts use the exporter type derived from the parent
 * elements `inner_exporter` method.
 *
 * Other than that, the logic is:
 *  - `SvgContext` for `<svg>` elements
 *  - `GContext` for `<g>` elements
 *  - `ShapeContext` for shape elements (`<path>`, `<rect>`, etc.)
 *  - `PatternContext` for <pattern>, but only with `ShapeContext` as a parent.
 */
struct ChildContextFactories {
    template <class ParentContext, class ElementTag>
    struct apply {
        using type = svgpp::factory::context::on_stack<
            typename detail::CCFImpl<ParentContext, ElementTag>::type>;
    };
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_FACTORIES_H_
