#include "shape.h"

#include <iostream>

#include "../../bezier.h"

/**
 * Error threshold for bezier subdivision.
 *
 * See `subdivideCurve` in `bezier.h` for details.
 */
constexpr double kBezierErrorThreshold = 5;

void draw_or_move(GpglExporter& exporter, bool draw, Point target) {
    if (draw) {
        exporter.draw_to(target);
    } else {
        exporter.move_to(target);
    }
}

ShapeContext::PathState& ShapeContext::assert_state() {
    if (state_) {
        return *state_;
    }

    // TODO(David): Error handling strategy
    throw std::runtime_error{"Invalid path: No leading move command"};
}

void ShapeContext::draw_line_to(Point target, ShapeContext::PathState& state) {
    if (dasharray_.empty()) {
        exporter().draw_to(target);
    } else {
        double remaining_dash =
            dasharray_[state.current_dash] - state.current_dash_progress;
        Point delta_vec = target - state.current_point;
        double remaining_line = delta_vec.norm();
        Point unit_vec = delta_vec.normalized();

        while (remaining_dash < remaining_line) {
            Point dash_end = state.current_point + remaining_dash * unit_vec;
            draw_or_move(exporter(), state.is_plotted_dash, dash_end);

            state.current_point = dash_end;
            state.current_dash = (state.current_dash + 1) % dasharray_.size();
            state.is_plotted_dash = !state.is_plotted_dash;
            state.current_dash_progress = 0;
            remaining_dash = dasharray_[state.current_dash];
            remaining_line -= remaining_dash;
        }

        draw_or_move(exporter(), state.is_plotted_dash, target);
        state.current_dash_progress += remaining_line;
        state.current_point = target;
    }
}

void ShapeContext::path_move_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    Point p = coordinate_system().to_root({x, y});
    exporter().move_to(p);
    state_ = PathState::reset_to(p);
}

void ShapeContext::path_line_to(double x, double y,
                                svgpp::tag::coordinate::absolute) {
    auto& state = assert_state();
    Point p = coordinate_system().to_root({x, y});
    draw_line_to(p, state);
}

void ShapeContext::path_cubic_bezier_to(double x1, double y1, double x2,
                                        double y2, double x, double y,
                                        svgpp::tag::coordinate::absolute) {
    auto& state = assert_state();
    Point ctrl1 = coordinate_system().to_root({x1, y1});
    Point ctrl2 = coordinate_system().to_root({x2, y2});
    Point end = coordinate_system().to_root({x, y});
    subdivideCurve(kBezierErrorThreshold, state.current_point, ctrl1, ctrl2,
                   end, [this, &state](Point p) { draw_line_to(p, state); });
}

void ShapeContext::path_close_subpath() {
    auto& state = assert_state();
    draw_line_to(state.start_point, state);
}

void ShapeContext::path_exit() {}

void ShapeContext::set(svgpp::tag::attribute::stroke_dasharray,
                       svgpp::tag::value::none) {
    dasharray_.clear();
}
