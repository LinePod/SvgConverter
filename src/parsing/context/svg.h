#ifndef SVG_CONVERTER_SVG_H
#define SVG_CONVERTER_SVG_H

#include "../coordinate_system.h"
#include "../viewport.h"
#include "graphics_element.h"

/**
 * Context for parsing <svg> elements.
 */
class SvgContext : public GraphicsElementContext {
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
    explicit SvgContext(GpglExporter exporter, const Viewport& global_viewport)
        : GraphicsElementContext(exporter, global_viewport, {}),
          // Per SVG spec the default width and height 100%, so the inner
          // viewport is effectively the same as the outer one.
          inner_viewport_{global_viewport} {}

    template <class ParentContext>
    explicit SvgContext(const ParentContext& parent)
        : GraphicsElementContext(parent),
          // See remark on the constructor above.
          inner_viewport_{viewport_} {}

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const;

    /**
     * SVG++ event reporting x, y, width and height properties.
     */
    void set_viewport(double, double, double width, double height);

    /**
     * SVG++ event reporting the viewbox size set by the `viewbox` attribute.
     */
    void set_viewbox_size(double width, double height);

    /**
     * SVG++ event reporting viewport width and/or height being set to 0.
     *
     * The SVG spec defines that the content rendering should be disabled in
     * this case.
     */
    void disable_rendering();

    /**
     * SVG++ event called when leaving an element.
     */
    void on_exit_element() {}

    /**
     * Whether rendering of this element and its children has been disabled.
     */
    bool rendering_disabled() const;
};

#endif  // SVG_CONVERTER_SVG_H
