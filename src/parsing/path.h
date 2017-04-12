#ifndef SVG_CONVERTER_PARSING_PATH_H
#define SVG_CONVERTER_PARSING_PATH_H

#include <type_traits>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "../bezier.h"
#include "../math_defs.h"
#include "dashes.h"

namespace detail {

/**
 * Error threshold for bezier subdivision.
 *
 * See `subdivide_curve` in `bezier.h` for details.
 */
constexpr double kBezierErrorThreshold = 5;

}  // namespace detail

struct MoveCommand {
    Vector target;
};

struct LineCommand {
    Vector target;
};

struct BezierCommand {
    Vector target;
    Vector control_point_1;
    Vector control_point_2;
};

struct CloseSubpathCommand {};

/**
 * Tagged enum over the possible commands in a path.
 *
 * These are reduced to a minimum based on the SVG++ path policy defined in
 * `traversal.h`.
 */
using PathCommand = boost::variant<MoveCommand, LineCommand, BezierCommand,
                                   CloseSubpathCommand>;

namespace detail {

/**
 * Visitor used to implement `Path::to_polylines`.
 */
template <class PolylineCallback>
class PathToPolylineVisitor : public boost::static_visitor<> {
 private:
    using PointCallback = std::result_of_t<PolylineCallback(Vector)>;

    struct SubpathState {
        PointCallback point_callback;
        Vector starting_point;
    };

    PolylineCallback polyline_callback_;
    Vector current_position_;

    /**
     * State concerning the subpath currently being converted.
     *
     * None if not currently in a subpath.
     */
    boost::optional<SubpathState> subpath_state_ = boost::none;

    SubpathState& assert_in_subpath();

 public:
    PathToPolylineVisitor(Vector start_position,
                          PolylineCallback polyline_callback);

    void operator()(const MoveCommand& command);
    void operator()(const LineCommand& command);
    void operator()(const BezierCommand& command);
    void operator()(const CloseSubpathCommand& command);
};

template <class PolylineCallback>
PathToPolylineVisitor<PolylineCallback>::PathToPolylineVisitor(
    Vector start_position, PolylineCallback polyline_callback)
    : polyline_callback_{polyline_callback},
      current_position_{start_position} {}

template <class PolylineCallback>
typename PathToPolylineVisitor<PolylineCallback>::SubpathState&
PathToPolylineVisitor<PolylineCallback>::assert_in_subpath() {
    if (!subpath_state_) {
        PointCallback point_callback = polyline_callback_(current_position_);
        // This weird construction is necessary to allow move only point
        // callbacks. All tries to use the = operator resulted in the compiler
        // trying to call a deleted copy assignment operator and not the move
        // assignment operator.
        subpath_state_.emplace(
            SubpathState{std::move(point_callback), current_position_});
    }

    return *subpath_state_;
}

template <class PolylineCallback>
void PathToPolylineVisitor<PolylineCallback>::operator()(
    const MoveCommand& command) {
    current_position_ = command.target;
    subpath_state_ = boost::none;
}

template <class PolylineCallback>
void PathToPolylineVisitor<PolylineCallback>::operator()(
    const LineCommand& command) {
    SubpathState& subpath_state = assert_in_subpath();
    subpath_state.point_callback(command.target);
    current_position_ = command.target;
}

template <class PolylineCallback>
void PathToPolylineVisitor<PolylineCallback>::operator()(
    const BezierCommand& command) {
    SubpathState& subpath_state = assert_in_subpath();
    subdivide_curve(kBezierErrorThreshold, current_position_,
                    command.control_point_1, command.control_point_2,
                    command.target, [&subpath_state](Vector point) {
                        subpath_state.point_callback(point);
                    });
    current_position_ = command.target;
}

template <class PolylineCallback>
void PathToPolylineVisitor<PolylineCallback>::operator()(
    const CloseSubpathCommand&) {
    subpath_state_->point_callback(subpath_state_->starting_point);
    current_position_ = subpath_state_->starting_point;
    subpath_state_ = boost::none;
}

MoveCommand transformed(MoveCommand command, const Transform& transform);

LineCommand transformed(LineCommand command, const Transform& transform);

BezierCommand transformed(BezierCommand command, const Transform& transform);

CloseSubpathCommand transformed(CloseSubpathCommand command, const Transform&);

/**
 * Implementation of path to polyline conversion without dashes.
 */
template <class PolylineCallback>
void path_to_polylines(const std::vector<PathCommand>& commands,
                       PolylineCallback polyline_callback) {
    if (commands.empty()) {
        return;
    }

    auto* move_cmd_ptr = boost::get<MoveCommand>(&commands.front());
    if (move_cmd_ptr == nullptr) {
        // TODO(David): Error handling strategy
        throw std::runtime_error{"Invalid path: No leading move command"};
    }

    detail::PathToPolylineVisitor<PolylineCallback> visitor{
        move_cmd_ptr->target, polyline_callback};
    for (const auto& command : commands) {
        boost::apply_visitor(visitor, command);
    }
}

}  // namespace detail

/**
 * An SVG path.
 *
 * Consists of several subpaths seperated by Move or SubpathClose commands.
 */
class Path {
 private:
    std::vector<PathCommand> commands_;

 public:
    /**
     * Extends the path by adding a command at the end.
     */
    void push_command(PathCommand command);

    /**
     * Apply a transformation to all commands in a path.
     */
    void transform(const Transform& transform);

    /**
     * Convert a path to a series of polylines.
     *
     * @param dasharray SVG dasharray to apply to all parts of the path. When
     *                  empty, all polylines will be solid.
     * @param polyline_callback Called with the first parameter of a new
     *                          polyline as the parameter. Must return a
     *                          callback which will be called once for each
     *                          subsequent point on the polyline.
     */
    template <class PolylineCallback>
    void to_polylines(const std::vector<double>& dasharray,
                      PolylineCallback polyline_callback) const;
};

template <class PolylineCallback>
void Path::to_polylines(const std::vector<double>& dasharray,
                        PolylineCallback polyline_callback) const {
    if (dasharray.empty()) {
        detail::path_to_polylines(commands_, polyline_callback);
    } else {
        // We use the same function but run the results through a dashifier

        // Move the polyline callback into this one to allow move only polyline
        // callbacks.
        auto dash_callback = [polyline_callback = std::move(polyline_callback)](
            Vector start, Vector end) {
            auto point_callback = polyline_callback(start);
            point_callback(end);
        };

        using Dashifier = PolylineDashifier<decltype(dash_callback)>;

        auto dashifying_polyline_callback = [dash_callback,
                                             &dasharray](Vector start_point) {
            Dashifier dashifier{dash_callback, start_point, dasharray};
            return [dashifier = std::move(dashifier)](Vector point) mutable {
                dashifier.process(point);
            };
        };
        detail::path_to_polylines(commands_, dashifying_polyline_callback);
    }
}

#endif  // SVG_CONVERTER_PARSING_PATH_H
