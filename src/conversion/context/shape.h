#ifndef SVG_CONVERTER_SHAPE_H
#define SVG_CONVERTER_SHAPE_H

#include <svgpp/definitions.hpp>

#include "./base.h"

using Point = Eigen::Vector2d;

class ShapeContext : public BaseContext {
 private:
    /**
     * Creates a point and transforms it to global space.
     */
    Point to_global_point(double x, double y);

 public:
    explicit ShapeContext(const BaseContext& parent);

    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute);
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute);
    void path_cubic_bezier_to(
            double x1, double y1,
            double x2, double y2,
            double x, double y,
            svgpp::tag::coordinate::absolute);
    void path_close_subpath();
    void path_exit();
};

#endif  // SVG_CONVERTER_SHAPE_H
