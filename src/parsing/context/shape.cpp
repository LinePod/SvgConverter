#include "shape.h"

void ShapeContext::path_move_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {}

void ShapeContext::path_line_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {}

void ShapeContext::path_cubic_bezier_to(double x1, double y1, double x2,
                                        double y2, double x, double y,
                                        svgpp::tag::coordinate::absolute) {}

void ShapeContext::path_close_subpath() {}

void ShapeContext::path_exit() {}
