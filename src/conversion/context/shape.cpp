#include "shape.h"

#include <iostream>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

using point3_t = boost::numeric::ublas::c_vector<double, 3>;

point_t ShapeContext::to_global_point(double x, double y) {
    point3_t point3;
    point3 <<= x, y, 1;
    point3 = boost::numeric::ublas::prod(transform_, point3);
    return boost::numeric::ublas::subrange(point3, 0, 2);
}

ShapeContext::ShapeContext(const BaseContext& parent) : BaseContext(parent) {
    std::cout << "Starting path\n";
}

void ShapeContext::path_move_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    auto p = to_global_point(x, y);
    std::cout << "Moving to " << p << '\n';
}

void ShapeContext::path_line_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    auto p = to_global_point(x, y);
    std::cout << "Line to " << p << '\n';
}

void ShapeContext::path_cubic_bezier_to(double x1, double y1, double x2,
                                        double y2, double x, double y,
                                        svgpp::tag::coordinate::absolute) {
    auto ctrl1 = to_global_point(x1, y1);
    auto ctrl2 = to_global_point(x2, y2);
    auto target = to_global_point(x, y);
    std::cout << "Cubic bezier curve to " << target << " with control points "
              << ctrl1 << " and " << ctrl2 << '\n';
}

void ShapeContext::path_close_subpath() {
    std::cout << "Closing subpath\n";
}

void ShapeContext::path_exit() {
    std::cout << "Closing path\n";
}
