#ifndef SVG_CONVERTER_G_H
#define SVG_CONVERTER_G_H

#include "graphics_element.h"

/**
 * Context for parsing <g> elements.
 */
class GContext : GraphicsElementContext {
 public:
    template <class ParentContext>
    explicit GContext(const ParentContext& parent)
        : GraphicsElementContext{parent} {}

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const;
};

#endif  // SVG_CONVERTER_G_H
