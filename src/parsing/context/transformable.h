#ifndef SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_

#include <boost/array.hpp>

#include "../coordinate_system.h"

/**
 * Context base class for elements which support the `transform` attribute.
 *
 * Through SVG++s automatic translation also supports the `patternTransform` and
 * `gradientTransform`. Also, due to the chosen viewport policy, transformations
 * from the x, y and viewBox attributes are reported like normal transforms.
 */
class TransformableContext {
 private:
    /**
     * Coordinate system for this element.
     *
     * This includes transforms from the element itself.
     */
    CoordinateSystem coordinate_system_;

 protected:
    explicit TransformableContext(CoordinateSystem coordinate_system);

    /**
     * Provides the derived classes read-only access to the coordinate system.
     */
    const CoordinateSystem& coordinate_system() const;

 public:
    /**
     * Handle a transform being reported by SVG++.
     *
     * This transforms the coordinate system for this element.
     */
    void transform_matrix(const boost::array<double, 6>& matrix);
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_
