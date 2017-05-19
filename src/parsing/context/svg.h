#ifndef SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
#define SVG_CONVERTER_PARSING_CONTEXT_SVG_H_

#include "../viewport.h"
#include "graphics_element.h"
#include "viewport_establishing.h"

/**
 * Context for parsing <svg> elements.
 */
template <class Exporter>
class SvgContext : public GraphicsElementContext<Exporter>,
                   public ViewportEstablishingContext {
 public:
    /**
     * Creates an SVG context for the root <svg> object.
     *
     * @param global_viewport Global viewport representing the available space.
     */
    explicit SvgContext(const SvgDocument& document, spdlog::logger& logger,
                        Exporter exporter, const Viewport& global_viewport)
        : GraphicsElementContext<Exporter>{document,
                                           logger,
                                           exporter,
                                           global_viewport,
                                           Transform::Identity()},
          // Per SVG spec the default width and height is 100%, so the inner
          // viewport is effectively the same as the outer one.
          ViewportEstablishingContext{global_viewport} {}

    template <class ParentContext>
    explicit SvgContext(ParentContext& parent)
        : GraphicsElementContext<Exporter>{parent},
          // See remark on the constructor above.
          ViewportEstablishingContext{this->viewport_} {}
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
