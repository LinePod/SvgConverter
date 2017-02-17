#ifndef SVG_CONVERTER_POLICIES_H
#define SVG_CONVERTER_POLICIES_H

#include "context/svg.h"

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

#endif  // SVG_CONVERTER_POLICIES_H
