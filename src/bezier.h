#ifndef SVG_CONVERTER_BEZIER_H
#define SVG_CONVERTER_BEZIER_H

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
void subdivideCurve(double error_threshold, Point start, Point ctrl1,
                    Point ctrl2, Point end, Callback callback) {
    using Line = Eigen::ParametrizedLine<double, 2>;

    Line straightLine{start, end - start};
    double error = straightLine.distance(ctrl1) + straightLine.distance(ctrl2);
    if (error > error_threshold) {
        // De Casteljau subdivision
        Point mid12 = (start + ctrl1) / 2;
        Point mid23 = (ctrl1 + ctrl2) / 2;
        Point mid34 = (ctrl2 + end) / 2;
        Point mid123 = (mid12 + mid23) / 2;
        Point mid234 = (mid23 + mid34) / 2;
        Point mid1234 = (mid123 + mid234) / 2;
        subdivideCurve(error_threshold, start, mid12, mid123, mid1234,
                       callback);
        subdivideCurve(error_threshold, mid1234, mid234, mid34, end, callback);
    } else {
        callback(end);
    }
}

#endif  // SVG_CONVERTER_BEZIER_H
