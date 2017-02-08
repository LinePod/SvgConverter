#ifndef SVG_CONVERTER_FACTORY_H
#define SVG_CONVERTER_FACTORY_H

#include <type_traits>

#include <svgpp/traits/element_groups.hpp>

#include "base.h"
#include "shape.h"

template<class ElementTag> constexpr bool is_shape_element_v =
        boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag>::value;

/**
 * Context factories to choose the context type for each svg element.
 */
struct ContextFactories {
    /**
     * Default implementation using `BaseContext`.
     */
    template<class ParentContext, class ElementTag, class Enable = void>
    struct apply {
        using type = svgpp::factory::context::on_stack<BaseContext>;
    };
};

/**
 * Specialization to use `ShapeContext` for all shape elements.
 */
template<class ElementTag>
struct ContextFactories::apply<
        BaseContext,
        ElementTag,
        std::enable_if_t<is_shape_element_v<ElementTag>>> {
    using type = svgpp::factory::context::on_stack<ShapeContext>;
};

#endif  // SVG_CONVERTER_FACTORY_H
