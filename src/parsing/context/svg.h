#ifndef SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
#define SVG_CONVERTER_PARSING_CONTEXT_SVG_H_

#include "../coordinate_system.h"
#include "../viewport.h"
#include "graphics_element.h"

/**
 * Context for parsing <svg> elements.
 */
template <class Exporter>
class SvgContext : public GraphicsElementContext<Exporter> {
 private:
    /**
     * New viewport for child elements established by this <svg> element.
     */
    Viewport inner_viewport_;

    bool rendering_disabled_ = false;

 public:
    /**
     * Creates an SVG context for the root <svg> object.
     *
     * @param global_viewport Global viewport representing the available space.
     */
    explicit SvgContext(const SvgDocument& document, Exporter exporter,
                        const Viewport& global_viewport)
        : GraphicsElementContext<Exporter>(document, exporter, global_viewport,
                                           {}),
          // Per SVG spec the default width and height is 100%, so the inner
          // viewport is effectively the same as the outer one.
          inner_viewport_{global_viewport} {}

    template <class ParentContext>
    explicit SvgContext(const ParentContext& parent)
        : GraphicsElementContext<Exporter>(parent),
          // See remark on the constructor above.
          inner_viewport_{this->viewport_} {}

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const { return this->inner_viewport_; }

    /**
     * SVG++ event reporting x, y, width and height properties.
     */
    void set_viewport(double, double, double width, double height) {
        // X and y, as well as all scaling and alignment is handled by SVG++ and
        // passed as a transform due to the viewport policy `as_transform`.
        inner_viewport_.set_size(width, height);
    }

    /**
     * SVG++ event reporting the viewbox size set by the `viewbox` attribute.
     */
    void set_viewbox_size(double width, double height) {
        inner_viewport_.set_size(width, height);
    }

    /**
     * SVG++ event reporting viewport width and/or height being set to 0.
     *
     * The SVG spec defines that the content rendering should be disabled in
     * this case.
     */
    void disable_rendering() { rendering_disabled_ = true; }

    /**
     * Whether rendering of this element and its children has been disabled.
     *
     * Used to disable traversal of child elements.
     */
    bool rendering_disabled() const { return rendering_disabled_; }
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
