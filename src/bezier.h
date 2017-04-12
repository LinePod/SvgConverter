#ifndef SVG_CONVERTER_BEZIER_H_
#define SVG_CONVERTER_BEZIER_H_

#include <eigen3/Eigen/Geometry>

#include <vector>

#include "math_defs.h"

/**
 * Subdivide the curve to create a polyline.
 *
 * We subdivide the curve until the error threshold is reached, at which point
 * we replace the subcurve by a straight line. As an error metric we use the
 * sum of the distances of both control points from the straight line between
 * the start and end point, as described in
 * http://www.antigrain.com/research/adaptive_bezier/index.html.
 *
 * The callback is called with a `Point` argument for each point in the
 * generated polyline (from start to back, excluding the start point).
 */
template <class Callback>
void subdivide_curve(double error_threshold, Vector start, Vector ctrl1,
                     Vector ctrl2, Vector end, Callback callback) {
    using Line = Eigen::ParametrizedLine<double, 2>;

    Line straight_line{start, end - start};
    double error =
        straight_line.distance(ctrl1) + straight_line.distance(ctrl2);
    if (error > error_threshold) {
        // De Casteljau subdivision
        Vector mid12 = (start + ctrl1) / 2;
        Vector mid23 = (ctrl1 + ctrl2) / 2;
        Vector mid34 = (ctrl2 + end) / 2;
        Vector mid123 = (mid12 + mid23) / 2;
        Vector mid234 = (mid23 + mid34) / 2;
        Vector mid1234 = (mid123 + mid234) / 2;
        subdivide_curve(error_threshold, start, mid12, mid123, mid1234,
                        callback);
        subdivide_curve(error_threshold, mid1234, mid234, mid34, end, callback);
    } else {
        callback(end);
    }
}

#endif  // SVG_CONVERTER_BEZIER_H_
