#ifndef SVG_CONVERTER_PARSING_CONTEXT_G_H_
#define SVG_CONVERTER_PARSING_CONTEXT_G_H_

#include "graphics_element.h"

/**
 * Context for parsing <g> elements.
 */
template <class Exporter>
class GContext : public GraphicsElementContext<Exporter> {
 public:
    template <class ParentContext>
    explicit GContext(ParentContext& parent);
};

template <class Exporter>
template <class ParentContext>
GContext<Exporter>::GContext(ParentContext& parent)
    : GraphicsElementContext<Exporter>{parent} {}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_G_H_
