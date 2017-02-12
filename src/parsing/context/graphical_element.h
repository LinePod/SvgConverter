#ifndef SVG_CONVERTER_LAYOUTCONTEXT_H
#define SVG_CONVERTER_LAYOUTCONTEXT_H

#include <boost/array.hpp>

#include "../coordinate_system.h"
#include "../viewport.h"

/**
 * Context base class for elements which directly render graphics.
 *
 * This includes containers like <svg> and <g>, but not things like <pattern>.
 *
 * These elements are always part of a viewport and a certain coordinate system,
 * which is created by transformations on this element or outer elements.
 *
 * Provides some layout related handlers/accessors for SVG++.
 *
 * See also the [SVGGraphicsElement]
 * (https://developer.mozilla.org/en-US/docs/Web/API/SVGGraphicsElement) from
 * SVG 2.
 */
class GraphicsElementContext {
 private:
    /**
     * Viewport to which this element belongs.
     *
     * Usually established by the next <svg> ancestor. Equal to the global
     * viewport for the outermost <svg> element.
     */
    const Viewport& viewport_;

    /**
     * Coordinate system for this element.
     *
     * This includes transforms from the element itself.
     */
    CoordinateSystem coordinate_system_;

 protected:
    GraphicsElementContext(const Viewport& viewport,
                  const CoordinateSystem& coordinate_system)
            : viewport_{viewport}, coordinate_system_{coordinate_system} {
    }

    /**
     * Creates a new instance from information supplied by a parent context.
     *
     * The parent class must have accessible methods `inner_viewport()` and
     * `inner_coordinate_system()`.
     */
    template<class ParentContext>
    explicit GraphicsElementContext(const ParentContext& parent)
            : viewport_{parent.inner_viewport()},
              coordinate_system_{parent.inner_coordinate_system()} {
    }

    /**
     * Accessor to the coordinate system of this element.
     */
    const CoordinateSystem& coordinate_system() const;

 public:
    /**
     * Handle a `transform` attribute being reported by svg++.
     *
     * This transforms the coordinate system for this element.
     */
    void transform_matrix(const boost::array<double, 6>& matrix);

    /**
     * Provides a length factory for SVG++ to resolve units.
     *
     * Based on the viewport the element is in.
     */
     const LengthFactory& length_factory() const;
};

#endif  // SVG_CONVERTER_LAYOUTCONTEXT_H
