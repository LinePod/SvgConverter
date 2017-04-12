#include "path.h"

#include <algorithm>

MoveCommand detail::transformed(MoveCommand command,
                                const Transform& transform) {
    return {transform * command.target};
}

LineCommand detail::transformed(LineCommand command,
                                const Transform& transform) {
    return {transform * command.target};
}

BezierCommand detail::transformed(BezierCommand command,
                                  const Transform& transform) {
    // Affine transformations can be applied to bezier curves by just applying
    // them to the control points (see http://math.stackexchange.com/a/1327062).
    return {transform * command.target, transform * command.control_point_1,
            transform * command.control_point_2};
}

CloseSubpathCommand detail::transformed(CloseSubpathCommand command,
                                        const Transform&) {
    return command;
}

void Path::push_command(PathCommand command) { commands_.push_back(command); }

void Path::transform(const Transform& transform) {
    std::transform(
        std::begin(commands_), std::end(commands_), std::begin(commands_),
        [transform](const PathCommand& command) {
            return boost::apply_visitor(
                [transform](auto&& cmd) {
                    return PathCommand{detail::transformed(cmd, transform)};
                },
                command);
        });
}
