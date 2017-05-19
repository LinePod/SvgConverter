#ifndef SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_

#include <boost/array.hpp>

#include "../../math_defs.h"

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
     * Transformation from the local coordinate system to the root one.
     *
     * The root coordinate system is one where each unit is a millimeter
     * and the (0, 0) point is on the top left of the paper.
     */
    Transform to_root_;

 protected:
    explicit TransformableContext(const Transform& to_root);

 public:
    /**
     * Provides read only access to the transform to the root coordinate system.
     */
    const Transform& to_root() const;

    /**
     * Handle a transform being reported by SVG++.
     */
    void transform_matrix(const boost::array<double, 6>& matrix);
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_TRANSFORMABLE_H_
