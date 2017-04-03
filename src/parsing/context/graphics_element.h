#ifndef SVG_CONVERTER_PARSING_CONTEXT_GRAPHICS_ELEMENT_H_
#define SVG_CONVERTER_PARSING_CONTEXT_GRAPHICS_ELEMENT_H_

#include <boost/array.hpp>

#include "../coordinate_system.h"
#include "../viewport.h"
#include "base.h"
#include "transformable.h"

/**
 * Context base class for elements which directly render graphics.
 *
 * This includes containers like <svg> and <g>, but not things like <pattern>.
 *
 * These elements are always part of a viewport and a certain coordinate system,
 * which is created by transformations on this element or outer elements. They
 * also have an exporter which is used to report generated lines for the
 * contained shapes.
 *
 * Provides some general layout related handlers/accessors for SVG++.
 *
 * See also the [SVGGraphicsElement]
 * (https://developer.mozilla.org/en-US/docs/Web/API/SVGGraphicsElement) from
 * SVG 2.
 */
template <class Exporter>
class GraphicsElementContext : public BaseContext, public TransformableContext {
 protected:
    /**
     * Exporter used to report generated lines.
     */
    Exporter exporter_;

    /**
     * Viewport to which this element belongs.
     *
     * Usually established by the next <svg> ancestor. Equal to the global
     * viewport for the outermost <svg> element.
     */
    const Viewport& viewport_;

    GraphicsElementContext(const SvgDocument& document, Exporter exporter,
                           const Viewport& viewport,
                           const CoordinateSystem& coordinate_system)
        : BaseContext(document),
          TransformableContext{coordinate_system},
          exporter_{exporter},
          viewport_{viewport} {}

    /**
     * Creates a new instance from information supplied by a parent context.
     *
     * The parent class must derive from `BaseContext` and have accessible
     * methods `inner_viewport()`, `inner_coordinate_system()` and
     * `inner_exporter()`.
     */
    template <class ParentContext>
    explicit GraphicsElementContext(ParentContext& parent)
        : BaseContext(parent),
          TransformableContext{parent.inner_coordinate_system()},
          exporter_{parent.inner_exporter()},
          viewport_{parent.inner_viewport()} {}

 public:
    /**
     * Provides a length factory for SVG++ to resolve units.
     *
     * Based on the viewport the element is in.
     */
    const LengthFactory& length_factory() const {
        return viewport_.length_factory();
    }

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     *
     * @return Coordinate system for child elements.
     */
    const CoordinateSystem& inner_coordinate_system() const {
        return coordinate_system();
    }

    /**
     * Provides the exporter for child elements.
     *
     * All graphics elements use the same exporter for their children that was
     * used for them.
     */
    Exporter inner_exporter() const { return exporter_; }
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_GRAPHICS_ELEMENT_H_
