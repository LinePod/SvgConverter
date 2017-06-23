#ifndef SVG_CONVERTER_PARSING_CONTEXT_G_H_
#define SVG_CONVERTER_PARSING_CONTEXT_G_H_

#include "../viewport.h"
#include "base.h"

/**
 * Context for parsing <g> elements.
 */
template <class Exporter>
class GContext : public BaseContext<Exporter> {
 public:
    template <class ParentContext>
    explicit GContext(ParentContext& parent);

    /**
     * Used by `BaseContext` to select the viewport for child elements.
     */
    const Viewport& inner_viewport() const { return this->viewport(); }

    /**
     * Used by `BaseContext` to select the exporter for child elements.
     */
    Exporter inner_exporter() const { return this->exporter_; }

    /**
     * Whether child elements should be processed.
     */
    bool process_children() const { return true; }

    /**
     * SVG++ event fired when the element has been fully processed.
     */
    void on_exit_element() {}
};

template <class Exporter>
template <class ParentContext>
GContext<Exporter>::GContext(ParentContext& parent)
    : BaseContext<Exporter>{parent} {}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_G_H_
