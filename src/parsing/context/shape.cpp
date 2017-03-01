#include "shape.h"

#include <iostream>

#include "../../bezier.h"

/**
 * Error threshold for bezier subdivision.
 *
 * See `subdivideCurve` in `bezier.h` for details.
 */
constexpr double bezier_error_threshold = 5;

ShapeContext::PathState& ShapeContext::assert_state() {
    if (state_) {
        return *state_;
    }

    // TODO(David): Error handling strategy
    throw std::runtime_error{"Invalid path: No leading move command"};
}

void ShapeContext::path_move_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    Point p = coordinate_system().to_root({x, y});
    exporter().move_to(p);
    state_ = PathState{p, p};
}

void ShapeContext::path_line_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    auto& state = assert_state();
    Point p = coordinate_system().to_root({x, y});
    exporter().draw_to(p);
    state.current_ = p;
}

void ShapeContext::path_cubic_bezier_to(double x1, double y1, double x2,
                                        double y2, double x, double y,
                                        svgpp::tag::coordinate::absolute) {
    auto& state = assert_state();
    Point ctrl1 = coordinate_system().to_root({x1, y1});
    Point ctrl2 = coordinate_system().to_root({x2, y2});
    Point end = coordinate_system().to_root({x, y});
    subdivideCurve(bezier_error_threshold, state.current_, ctrl1, ctrl2, end,
                   [this](Point p) { exporter().draw_to(p); });

    state.current_ = end;
}

void ShapeContext::path_close_subpath() {
    auto& state = assert_state();
    exporter().draw_to(state.current_);
    state.current_ = state.start_;
}

void ShapeContext::path_exit() {}
