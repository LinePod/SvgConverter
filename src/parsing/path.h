#ifndef SVG_CONVERTER_PARSING_PATH_H
#define SVG_CONVERTER_PARSING_PATH_H

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "../bezier.h"
#include "../math_defs.h"

namespace detail {

/**
 * Error threshold for bezier subdivision.
 *
 * See `subdivide_curve` in `bezier.h` for details.
 */
constexpr double kBezierErrorThreshold = 5;

}  // namespace detail

struct InvalidPathError : std::exception {
    const char* what() const noexcept override;
};

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
 * Command visitor used to implement `Path::to_polylines`.
 */
template <class PolylineVisitorFactory>
class PathToPolylineVisitor : public boost::static_visitor<> {
 private:
    using PolylineVisitor = std::result_of_t<PolylineVisitorFactory(Vector)>;

    struct SubpathState {
        PolylineVisitor visitor;
        Vector starting_point;
    };

    PolylineVisitorFactory visitor_factory_;
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
                          PolylineVisitorFactory polyline_visitor_factory);

    void operator()(const MoveCommand& command);
    void operator()(const LineCommand& command);
    void operator()(const BezierCommand& command);
    void operator()(const CloseSubpathCommand& command);
};

template <class PolylineVisitorFactory>
PathToPolylineVisitor<PolylineVisitorFactory>::PathToPolylineVisitor(
    Vector start_position, PolylineVisitorFactory polyline_visitor_factory)
    : visitor_factory_{polyline_visitor_factory},
      current_position_{std::move(start_position)} {}

template <class PolylineVisitorFactory>
typename PathToPolylineVisitor<PolylineVisitorFactory>::SubpathState&
PathToPolylineVisitor<PolylineVisitorFactory>::assert_in_subpath() {
    if (!subpath_state_) {
        PolylineVisitor visitor = visitor_factory_(current_position_);
        // This weird construction is necessary to allow move only point
        // callbacks. All tries to use the = operator resulted in the compiler
        // trying to call a deleted copy assignment operator and not the move
        // assignment operator.
        subpath_state_.emplace(
            SubpathState{std::move(visitor), current_position_});
    }

    return *subpath_state_;
}

template <class PolylineVisitorFactory>
void PathToPolylineVisitor<PolylineVisitorFactory>::operator()(
    const MoveCommand& command) {
    current_position_ = command.target;
    subpath_state_ = boost::none;
}

template <class PolylineVisitorFactory>
void PathToPolylineVisitor<PolylineVisitorFactory>::operator()(
    const LineCommand& command) {
    SubpathState& subpath_state = assert_in_subpath();
    subpath_state.visitor(command.target);
    current_position_ = command.target;
}

template <class PolylineVisitorFactory>
void PathToPolylineVisitor<PolylineVisitorFactory>::operator()(
    const BezierCommand& command) {
    SubpathState& subpath_state = assert_in_subpath();
    subdivide_curve(
        kBezierErrorThreshold, current_position_, command.control_point_1,
        command.control_point_2, command.target,
        [&subpath_state](Vector point) { subpath_state.visitor(point); });
    current_position_ = command.target;
}

template <class PolylineVisitorFactory>
void PathToPolylineVisitor<PolylineVisitorFactory>::operator()(
    const CloseSubpathCommand& /*unused*/) {
    subpath_state_->visitor(subpath_state_->starting_point);
    current_position_ = subpath_state_->starting_point;
    subpath_state_ = boost::none;
}

MoveCommand transformed(MoveCommand command, const Transform& transform);

LineCommand transformed(LineCommand command, const Transform& transform);

BezierCommand transformed(BezierCommand command, const Transform& transform);

CloseSubpathCommand transformed(CloseSubpathCommand command,
                                const Transform& /*unused*/);

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
    void push_command(const PathCommand& command);

    /**
     * Apply a transformation to all commands in a path.
     */
    void transform(const Transform& transform);

    /**
     * Convert a path to a series of polylines.
     *
     * @param polyline_visitor_factory Factory to create a visitor for a
     *                                 polyline. The factory will be called with
     *                                 the starting point as the only parameter.
     *                                 It must return a callable visitor, which
     *                                 will be called for each point in the
     *                                 polyline. When the polyline is finished,
     *                                 the visitor will be destructed.
     */
    template <class PolylineVisitorFactory>
    void to_polylines(PolylineVisitorFactory polyline_visitor_factory) const;
};

template <class PolylineVisitorFactory>
void Path::to_polylines(PolylineVisitorFactory polyline_visitor_factory) const {
    if (commands_.empty()) {
        return;
    }

    auto* move_cmd_ptr = boost::get<MoveCommand>(&commands_.front());
    if (move_cmd_ptr == nullptr) {
        throw InvalidPathError{};
    }

    detail::PathToPolylineVisitor<PolylineVisitorFactory> command_visitor{
        move_cmd_ptr->target, polyline_visitor_factory};
    for (const auto& command : commands_) {
        boost::apply_visitor(command_visitor, command);
    }
}

#endif  // SVG_CONVERTER_PARSING_PATH_H
