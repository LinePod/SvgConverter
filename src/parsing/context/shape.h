#ifndef SVG_CONVERTER_SHAPE_H
#define SVG_CONVERTER_SHAPE_H

#include <boost/optional.hpp>
#include <svgpp/definitions.hpp>

#include "../../math_defs.h"
#include "graphics_element.h"

/**
 * Context for shape elements, like <path> or <rect>.
 *
 * SVG++ automatically converts all shapes to paths and then to a minimal subset
 * of the path commands, so that we only need to implement a few methods.
 */
class ShapeContext : public GraphicsElementContext {
 private:
    struct PathState {
        Point start_;
        Point current_;
    };

    /**
     * State of path parsing.
     *
     * Contains the first point of the current subpath and the current. Only
     * none before the mandatory first move command has been parsed.
     */
    boost::optional<PathState> state_;

    /**
     * Returns to the path state or throws an error if is not initialized.
     */
    PathState& assert_state();

 public:
    template <class ParentContext>
    explicit ShapeContext(const ParentContext& parent)
        : GraphicsElementContext{parent} {}

    /**
     * SVG++ event for a non drawn movement in a shape path.
     */
    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute);

    /**
     * SVG++ event for a straight line in a shape path.
     */
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute);

    /**
     * SVG++ event for a cubic bezier part of a shape path.
     */
    void path_cubic_bezier_to(double x1, double y1, double x2, double y2,
                              double x, double y,
                              svgpp::tag::coordinate::absolute);

    /**
     * SVG++ event for a straight line to the start of the current subpath.
     */
    void path_close_subpath();

    /**
     * SVG++ event after the last shape command.
     */
    void path_exit();

    /**
     * Svg++ event called when leaving an element.
     */
    void on_exit_element() {}
};

#endif  // SVG_CONVERTER_SHAPE_H
