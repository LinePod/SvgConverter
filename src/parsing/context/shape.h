#ifndef SVG_CONVERTER_SHAPE_H
#define SVG_CONVERTER_SHAPE_H

#include <vector>

#include <boost/optional.hpp>
#include <boost/range.hpp>
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
    /**
     * State maintained while parsing the shape/path of the shape.
     */
    struct PathState {
        /**
         * Start point of the current subpath.
         */
        Point start_point;

        /**
         * Current point of the path.
         */
        Point current_point;

        /**
         * Index of the entry in `dasharray` which is currently being applied.
         */
        std::size_t current_dash;

        /**
         * How much of the current dash has already been plotted.
         */
        double current_dash_progress;

        /**
         * Whether the current dash is a plotted one or an empty one.
         */
        bool is_plotted_dash;

        /**
         * A reset `PathState` for a new subpath starting at the given point.
         */
        static PathState reset_to(Point p) {
            return PathState{p, p, 0, 0, true};
        }
    };

    /**
     * State of path parsing.
     *
     * Contains the first point of the current subpath and the current. Only
     * none before the mandatory first move command has been parsed.
     */
    boost::optional<PathState> state_;

    /**
     * Describes the pattern of the stroke, set by `stroke-dasharray`.
     */
    std::vector<double> dasharray_;

    /**
     * Returns to the path state or throws an error if is not initialized.
     */
    PathState& assert_state();

    /**
     * Draw a line with an already asserted state.
     */
    void draw_line_to(Point target, PathState& state);

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
     * SVG++ event called when leaving an element.
     */
    void on_exit_element() {}

    /**
     * SVG++ event when `stroke-dasharray` is set to an emtpy value.
     */
    void set(svgpp::tag::attribute::stroke_dasharray, svgpp::tag::value::none);

    /**
     * SVG++ event when `stroke-dasharray` is set to a non empty value.
     */
    template <class Range>
    void set(svgpp::tag::attribute::stroke_dasharray, const Range& range) {
        dasharray_.assign(boost::begin(range), boost::end(range));
    }
};

#endif  // SVG_CONVERTER_SHAPE_H
