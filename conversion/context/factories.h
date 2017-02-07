#ifndef SVG_CONVERTER_FACTORY_H
#define SVG_CONVERTER_FACTORY_H

#include "base.h"

/**
 * Context factories to choose the context type for each svg element.
 */
struct ContextFactories {
    template<class ParentContext, class ElementTag>
    struct apply {
        typedef svgpp::factory::context::on_stack<BaseContext> type;
    };
};

#endif //SVG_CONVERTER_FACTORY_H
