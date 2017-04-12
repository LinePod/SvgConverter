#ifndef SVG_CONVERTER_PARSING_CONTEXT_SHAPE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_SHAPE_H_

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/mpl/set.hpp>
#include <svgpp/definitions.hpp>

#include "../../math_defs.h"
#include "../path.h"
#include "../traversal.h"
#include "graphics_element.h"
#include "pattern.h"

/**
 * Context for shape elements, like <path> or <rect>.
 *
 * SVG++ automatically converts all shapes to paths and then to a minimal subset
 * of the path commands, so that we only need to implement a few methods.
 */
template <class Exporter>
class ShapeContext : public GraphicsElementContext<Exporter> {
 private:
    /**
     * Saved shape path.
     */
    Path path_;

    /**
     * Describes the pattern of the stroke, set by `stroke-dasharray`.
     */
    std::vector<double> dasharray_;

    /**
     * IRI specified with the `fill` attribute.
     *
     * Empty if the element should not be filled. Even though the SVG standard
     * defaults `fill` to `black`, we default to not filling an element because
     * there is no good default fill.
     */
    std::string fill_fragment_iri_;

 public:
    template <class ParentContext>
    explicit ShapeContext(ParentContext& parent)
        : GraphicsElementContext<Exporter>{parent} {}

    /**
     * SVG++ event for a non drawn movement in a shape path.
     */
    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute) {
        path_.push_command(MoveCommand{{x, y}});
    }

    /**
     * SVG++ event for a straight line in a shape path.
     */
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
        path_.push_command(LineCommand{{x, y}});
    }

    /**
     * SVG++ event for a cubic bezier part of a shape path.
     */
    void path_cubic_bezier_to(double x1, double y1, double x2, double y2,
                              double x, double y,
                              svgpp::tag::coordinate::absolute) {
        path_.push_command(BezierCommand{{x, y}, {x1, y1}, {x2, y2}});
    }

    /**
     * SVG++ event for a straight line to the start of the current subpath.
     */
    void path_close_subpath() { path_.push_command(CloseSubpathCommand{}); }

    /**
     * SVG++ event after the last shape command.
     */
    void path_exit() {}

    void on_exit_element() {
        using ExpectedElements = boost::mpl::set1<svgpp::tag::element::pattern>;
        // Override the processed elements setting just for the referenced
        // element. Allows us to process <pattern> only when referenced.
        using ProcessedElements = ExpectedElements;

        path_.transform(this->coordinate_system().transform());

        if (!fill_fragment_iri_.empty()) {
            auto referenced_node =
                this->document_.find_by_id(fill_fragment_iri_);
            PatternPseudoContext<Exporter> context{
                *this, this->exporter_, this->viewport_,
                this->coordinate_system(), path_};
            DocumentTraversal::load_referenced_element<
                svgpp::expected_elements<ExpectedElements>,
                svgpp::processed_elements<ProcessedElements>>::
                load(referenced_node, context);
        }

        // We move the path and dasharray, so that an exporter can store them
        // for later use without copying.
        this->exporter_.plot(std::move(path_), std::move(dasharray_));
    }

    /**
     * SVG++ event when `stroke-dasharray` is set to an emtpy value.
     */
    void set(svgpp::tag::attribute::stroke_dasharray, svgpp::tag::value::none) {
        dasharray_.clear();
    }

    /**
     * SVG++ event when `stroke-dasharray` is set to a non empty value.
     */
    template <class Range>
    void set(svgpp::tag::attribute::stroke_dasharray, const Range& range) {
        dasharray_.assign(boost::begin(range), boost::end(range));
    }

    template <class... Args>
    void set(svgpp::tag::attribute::fill, Args...) {
        throw std::runtime_error{"Unsupported fill type"};
    }

    void set(svgpp::tag::attribute::fill, svgpp::tag::value::none) {
        fill_fragment_iri_.clear();
    }

    template <class String>
    void set(svgpp::tag::attribute::fill, svgpp::tag::iri_fragment,
             const String& id) {
        fill_fragment_iri_.assign(boost::begin(id), boost::end(id));
    }
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_SHAPE_H_
