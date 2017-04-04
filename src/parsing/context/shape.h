#ifndef SVG_CONVERTER_PARSING_CONTEXT_SHAPE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_SHAPE_H_

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/mpl/set.hpp>
#include <boost/optional.hpp>
#include <svgpp/definitions.hpp>

#include "../../bezier.h"
#include "../../math_defs.h"
#include "../traversal.h"
#include "graphics_element.h"
#include "pattern.h"

namespace detail {

/**
 * Error threshold for bezier subdivision.
 *
 * See `subdivideCurve` in `bezier.h` for details.
 */
constexpr double kBezierErrorThreshold = 5;

}  // namespace detail

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
     * Path of the outline of the shape.
     *
     * Each contained vector describes a subpath of the path.
     *
     * After parsing the last vector may contain a single point. In this case
     * that vector must be ignored for stroke and fill generation.
     *
     * In most cases this should only be accessed through `assert_subpath`.
     */
    std::vector<std::vector<Vector>> outline_path_;

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

    /**
     * Returns a reference to the current subpath.
     *
     * Throws an error if no starting point for the first subpath has been
     * specified. This happens if the data for a <path> does not start with a
     * move command, and is considered invalid.
     *
     * The returned subpath will always contain at least a starting point.
     */
    std::vector<Vector>& assert_subpath() {
        if (!outline_path_.empty()) {
            return outline_path_.back();
        }

        // TODO(David): Error handling strategy
        throw std::runtime_error{"Invalid path: No leading move command"};
    }

 public:
    template <class ParentContext>
    explicit ShapeContext(ParentContext& parent)
        : GraphicsElementContext<Exporter>{parent} {}

    /**
     * SVG++ event for a non drawn movement in a shape path.
     */
    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute) {
        Vector global_point = this->coordinate_system().to_root({x, y});
        if (outline_path_.empty()) {
            // This is the initial move command. We add a subpath with the
            // starting point
            outline_path_.push_back({global_point});
        } else {
            auto& subpath = assert_subpath();
            if (subpath.size() == 1) {
                // Empty subpath, set new starting point
                subpath[0] = global_point;
            } else {
                // Create new subpath at target position
                outline_path_.push_back({global_point});
            }
        }
    }

    /**
     * SVG++ event for a straight line in a shape path.
     */
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
        auto& subpath = assert_subpath();
        subpath.push_back(this->coordinate_system().to_root({x, y}));
    }

    /**
     * SVG++ event for a cubic bezier part of a shape path.
     */
    void path_cubic_bezier_to(double x1, double y1, double x2, double y2,
                              double x, double y,
                              svgpp::tag::coordinate::absolute) {
        auto& subpath = assert_subpath();
        Vector ctrl1 = this->coordinate_system().to_root({x1, y1});
        Vector ctrl2 = this->coordinate_system().to_root({x2, y2});
        Vector end = this->coordinate_system().to_root({x, y});
        subdivideCurve(detail::kBezierErrorThreshold, subpath.back(), ctrl1,
                       ctrl2, end,
                       [&subpath](Vector p) { subpath.push_back(p); });
    }

    /**
     * SVG++ event for a straight line to the start of the current subpath.
     */
    void path_close_subpath() {
        auto& subpath = assert_subpath();
        subpath.push_back(subpath.front());
    }

    /**
     * SVG++ event after the last shape command.
     */
    void path_exit() {}

    void on_exit_element() {
        for (auto& subpath : outline_path_) {
            if (subpath.size() > 1) {
                this->exporter_.plot(subpath, dasharray_);
            }
        }

        using ExpectedElements = boost::mpl::set1<svgpp::tag::element::pattern>;
        // Override the processed elements setting just for the referenced
        // element. Allows us to process <pattern> only when referenced.
        using ProcessedElements = ExpectedElements;

        if (!fill_fragment_iri_.empty()) {
            auto referenced_node =
                this->document_.find_by_id(fill_fragment_iri_);
            PatternPseudoContext<Exporter> context{
                *this, this->exporter_, this->viewport_,
                this->coordinate_system(), outline_path_};
            DocumentTraversal::load_referenced_element<
                svgpp::expected_elements<ExpectedElements>,
                svgpp::processed_elements<ProcessedElements>>::
                load(referenced_node, context);
        }
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
