#ifndef SVG_CONVERTER_G_H
#define SVG_CONVERTER_G_H

#include "graphics_element.h"

/**
 * Context for parsing <g> elements.
 */
template <class Exporter>
class GContext : public GraphicsElementContext<Exporter> {
 public:
    template <class ParentContext>
    explicit GContext(const ParentContext& parent)
        : GraphicsElementContext<Exporter>{parent} {}

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const { return this->viewport_; }

    /**
     * SVG++ event called when leaving an element.
     */
    void on_exit_element() {}
};

#endif  // SVG_CONVERTER_G_H
