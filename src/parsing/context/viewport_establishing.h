#ifndef SVG_CONVERTER_PARSING_CONTEXT_VIEWPORT_ESTABLISHING_H_
#define SVG_CONVERTER_PARSING_CONTEXT_VIEWPORT_ESTABLISHING_H_

#include <boost/optional.hpp>

#include "../viewport.h"

/**
 * Context base class for elements which establish a new viewport for children.
 *
 * This is all elements which support the x, y, width, height and viewBox
 * attributes.
 */
class ViewportEstablishingContext {
    /**
     * New viewport for child elements established by this element.
     *
     * None if rendering was disabled by a width and/or height of 0.
     */
    boost::optional<Viewport> inner_viewport_;

 public:
    /**
     * Creates a new context.
     *
     * @param inner_viewport The default inner viewport, or none, if rendering
     *                       is disabled by default.
     */
    explicit ViewportEstablishingContext(
        boost::optional<Viewport> inner_viewport);

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const;

    /**
     * SVG++ event reporting x, y, width and height properties.
     */
    void set_viewport(double /*unused*/, double /*unused*/, double width,
                      double height);

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
     * Whether rendering of this element and its children has been disabled.
     *
     * Used to disable traversal of child elements.
     */
    bool rendering_disabled() const;
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_VIEWPORT_ESTABLISHING_H_
