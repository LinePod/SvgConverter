#ifndef SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
#define SVG_CONVERTER_PARSING_CONTEXT_SVG_H_

#include "../viewport.h"
#include "base.h"

/**
 * Context for parsing <svg> elements.
 */
template <class Exporter>
class SvgContext : public BaseContext<Exporter> {
 private:
    /**
     * New viewport for child elements established by this element.
     *
     * None if rendering was disabled by a width and/or height of 0.
     */
    boost::optional<Viewport> inner_viewport_;

 public:
    /**
     * Creates an SVG context for the root <svg> object.
     *
     * @param global_viewport Global viewport representing the available space.
     */
    explicit SvgContext(const SvgDocument& document, spdlog::logger& logger,
                        Exporter exporter, const Viewport& global_viewport);

    template <class ParentContext>
    explicit SvgContext(ParentContext& parent);

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const {
        // This is only accessed from the constructor of a child element. If
        // `inner_viewport_` is not set, no child elements should ever be
        // traversed due to the `DocumentTraversalControlPolicy`.
        return *inner_viewport_;
    }

    /**
     * Used by `BaseContext` to select the exporter for child elements.
     */
    Exporter inner_exporter() const { return this->exporter_; }

    /**
     * Whether rendering of this element and its children has been disabled.
     *
     * Used to disable traversal of child elements when rendering has been
     * disabled with a width/height of zero.
     */
    bool process_children() const { return inner_viewport_ != boost::none; }

    /**
     * SVG++ event reporting x, y, width and height properties.
     */
    void set_viewport(double /*unused*/, double /*unused*/, double width,
                      double height) {
        // X and y, as well as all scaling and alignment is handled by SVG++ and
        // passed as a transform due to the viewport policy `as_transform`.
        inner_viewport_ = Viewport{width, height};
    }

    /**
     * SVG++ event reporting the viewbox size set by the `viewbox` attribute.
     */
    void set_viewbox_size(double width, double height) {
        inner_viewport_ = Viewport{width, height};
    }

    /**
     * SVG++ event reporting viewport width and/or height being set to 0.
     *
     * The SVG spec defines that the content rendering should be disabled in
     * this case.
     */
    void disable_rendering() { inner_viewport_ = boost::none; }

    /**
     * SVG++ event fired when the element has been fully processed.
     */
    void on_exit_element() {}
};

template <class Exporter>
SvgContext<Exporter>::SvgContext(const SvgDocument& document,
                                 spdlog::logger& logger, Exporter exporter,
                                 const Viewport& global_viewport)
    : BaseContext<Exporter>{document, logger, exporter, global_viewport,
                            Transform::Identity()},
      // Per SVG spec the default width and height is 100%, so the inner
      // viewport is effectively the same as the outer one.
      inner_viewport_{global_viewport} {}

template <class Exporter>
template <class ParentContext>
SvgContext<Exporter>::SvgContext(ParentContext& parent)
    : BaseContext<Exporter>{parent},
      // See remark on the constructor above.
      inner_viewport_{this->viewport()} {}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_SVG_H_
