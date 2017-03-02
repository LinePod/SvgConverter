#ifndef SVG_CONVERTER_FACTORIES_H
#define SVG_CONVERTER_FACTORIES_H

#include <type_traits>

#include <boost/mpl/has_key.hpp>
#include <svgpp/factory/context.hpp>
#include <svgpp/traits/element_groups.hpp>

#include "g.h"
#include "shape.h"
#include "svg.h"

template <class ElementTag>
constexpr bool kIsShapeElement =
    boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag>::value;

/**
 * Describes how to create context instances for each SVG element.
 *
 * We use `svgpp::factory::context::on_stack<>` for everything, meaning that
 * a new instance is created for each element, and that the instances will get
 * a reference to the parent context passed to their constructor.
 *
 * Other than that, the logic is:
 *  - `SvgContext` for `<svg>` elements
 *  - `GContext` for `<g>` elements
 *  - `ShapeContext` for shape elements (`<path>`, `<rect>`, etc.)
 */
struct ChildContextFactories {
    template <class ParentContext, class ElementTag, class Enable = void>
    struct apply {};
};

// Overload for `SvgContext`
template <class ParentContext>
struct ChildContextFactories::apply<ParentContext, svgpp::tag::element::svg> {
    using type = svgpp::factory::context::on_stack<SvgContext>;
};

// Overload for `GContext`
template <class ParentContext>
struct ChildContextFactories::apply<ParentContext, svgpp::tag::element::g> {
    using type = svgpp::factory::context::on_stack<GContext>;
};

// Overload for `ShapeContext`
template <class ParentContext, class ElementTag>
struct ChildContextFactories::apply<
    ParentContext, ElementTag, std::enable_if_t<kIsShapeElement<ElementTag>>> {
    using type = svgpp::factory::context::on_stack<ShapeContext>;
};

#endif  // SVG_CONVERTER_FACTORIES_H
