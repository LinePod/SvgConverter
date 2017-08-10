#ifndef SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_
#define SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_

#include <tuple>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <clipper.hpp>

#include "../../math_defs.h"
#include "../dashes.h"
#include "../path.h"
#include "../traversal.h"
#include "../viewport.h"
#include "base.h"
#include "shape.h"

namespace detail {

/**
 * Exports shapes to a list of paths, which can than be tiled later.
 */
class PatternExporter {
 private:
    std::vector<DashedPath>& paths_;

 public:
    /**
     * Create a new exporter.
     *
     * @param paths List of paths to write to. Reference must be valid for the
     *              lifetime of the exporter and its copies.
     */
    explicit PatternExporter(std::vector<DashedPath>& paths);

    /**
     * Export the given path using the given dasharray.
     *
     * If the dasharray is empty, the lines are drawn fully solid.
     */
    void plot(DashedPath path);
};

/**
 * Values for the patternUnits and patternContentUnits attributes.
 */
enum class UnitType { kUserSpaceOnUse, kObjectBoundingBox };

/**
 * Groups attributes responsible for determining the layout of a <pattern>.
 */
struct PatternLayoutAttributes {
    /**
     * Value of the attribute x.
     */
    boost::optional<double> x = boost::none;

    /**
     * Value of the attribute y.
     */
    boost::optional<double> y = boost::none;

    /**
     * Value of the attribute width.
     */
    boost::optional<double> width = boost::none;

    /**
     * Value of the attribute height.
     */
    boost::optional<double> height = boost::none;

    /**
     * Viewbox as set by the viewBox attribute.
     */
    boost::optional<Rect> viewbox = boost::none;

    boost::variant<svgpp::tag::value::none, svgpp::tag::value::xMinYMin,
                   svgpp::tag::value::xMinYMid, svgpp::tag::value::xMinYMax,
                   svgpp::tag::value::xMidYMin, svgpp::tag::value::xMidYMid,
                   svgpp::tag::value::xMidYMax, svgpp::tag::value::xMaxYMin,
                   svgpp::tag::value::xMaxYMid, svgpp::tag::value::xMaxYMax>
        align = svgpp::tag::value::xMidYMid{};

    boost::variant<svgpp::tag::value::meet, svgpp::tag::value::slice>
        meet_or_slice = svgpp::tag::value::meet{};

    UnitType pattern_units = UnitType::kObjectBoundingBox;
};

/**
 * Calculate the layout of a pattern.
 *
 * @param attribs Collected attributes set on the pattern element.
 * @param bbox_size Bounding box of the shape being tiled.
 * @param viewport Viewport to use for resolving units.
 * @return Transform to apply to the to_root transform as well as the size of
 *         the pattern in the pattern local coordinate system. None, if
 *         rendering should be disabled.
 */
boost::optional<std::tuple<Transform, Vector>> calculate_pattern_layout(
    const PatternLayoutAttributes& attribs, Vector bbox_size,
    const Viewport& viewport);

/*
 * Generate a tiling for a pattern to completely fill the given clipping path.
 *
 * @param pattern_size Size of the pattern rectangle in the coordinate system
 *                     established by to_root.
 * @param to_root Transform to the root coordinate system.
 * @param clipping_path Path in global coordinates that should be completely
 *                      tiled.
 * @return List of offsets in root space. If the pattern is repeated at all the
 *         given offsets, it will completely cover the given clipping path.
 *         If the pattern needs to be tiled at its original position (offset
 *         0,0), that will be included in the list as well.
 */
std::vector<Vector> compute_tiling_offsets(const Vector& pattern_size,
                                           const Transform& to_root,
                                           const Path& clipping_path);

ClipperLib::PolyTree clip_tiled_pattern(
    const Path& clipping_path, const std::vector<DashedPath>& pattern_paths,
    const std::vector<Vector>& offsets);

Vector from_clipper_point(ClipperLib::IntPoint point);

}  // namespace detail

/**
 * Context for <pattern> elements.
 *
 * This context will be created by SVG++ when passing a `ShapeContext`
 * and a <pattern> element to `load_referencing_element`.
 *
 * It needs shape specific information (like the current viewport and coordinate
 * system as well as the area being tiled with the pattern). If multiple shapes
 * use the same pattern, it is parsed once for each shape.
 */
template <class Exporter>
class PatternContext : public BaseContext<Exporter> {
 private:
    /**
     * Outline of the shape to fill with this pattern.
     *
     * Used to clip the pattern to the shape.
     */
    const Path& clipping_path_;

    /**
     * Filled with all paths in the pattern via `PatternExporter`.
     */
    std::vector<DashedPath> pattern_paths_;

    /**
     * Describes how the lengths in the contained shapes are interpreted.
     *
     * Set by the `patternContentUnits` attribute.
     */
    detail::UnitType pattern_content_units_ = detail::UnitType::kUserSpaceOnUse;

    /**
     * Collected layout attributes.
     */
    detail::PatternLayoutAttributes layout_attribs_;

    /**
     * Size of the pattern as set by the viewport and/or viewbox.
     *
     * Set after all viewport attributes have been parsed and before any other
     * attributes/children are parsed. See AttributeTraversalControlPolicy in
     * traversal.h for more details.
     *
     * If patternUnits="objectBoundingBox", this size does *not* contain the
     * upscaling by the size of the bounding box.
     */
    boost::optional<Vector> size_ = boost::none;

    /**
     * Reports a transformation like SVG++ would.
     *
     * Used for our custom parsing of the viewport attributes. See
     * ViewportPolicy in traversal.h for why we need that.
     */
    void report_transform(const Transform& transform) {
        this->transform_matrix(boost::array<double, 6>{
            {transform(0, 0), transform(1, 0), transform(0, 1), transform(1, 1),
             transform(0, 2), transform(1, 2)}});
    }

 public:
    template <class ParentExporter>
    explicit PatternContext(ShapeContext<ParentExporter>& shape_context);

    /**
     * Used by `BaseContext` to select the viewport for child elements.
     */
    const Viewport& inner_viewport() const {
        // <pattern> elements don't actually create a new viewport, percentages
        // are relative to the viewport of the <svg> element the <pattern> is
        // defined in. Right now they are interpreted as relative to the
        // viewport of the referencing element, which is an open bug.
        return this->viewport();
    }

    /**
     * Used by `BaseContext` to select the exporter for child elements.
     */
    detail::PatternExporter inner_exporter() {
        return detail::PatternExporter{pattern_paths_};
    }

    /**
     * Event send after all viewport attributes have been parsed.
     *
     * Send before any other attributes/children have been parsed.
     */
    bool notify(AfterViewportAttributesEvent /*unused*/);

    /**
     * Whether child elements should be processed.
     */
    bool process_children() const { return size_ != boost::none; }

    /**
     * SVG++ event reporting the value of the attribute x.
     */
    void set(svgpp::tag::attribute::x /*unused*/, double value) {
        layout_attribs_.x = value;
    }

    /**
     * SVG++ event reporting the value of the attribute y.
     */
    void set(svgpp::tag::attribute::y /*unused*/, double value) {
        layout_attribs_.y = value;
    }

    /**
     * SVG++ event reporting the value of the attribute width.
     */
    void set(svgpp::tag::attribute::width /*unused*/, double value) {
        layout_attribs_.width = value;
    }

    /**
     * SVG++ event reporting the value of the attribute height.
     */
    void set(svgpp::tag::attribute::height /*unused*/, double value) {
        layout_attribs_.height = value;
    }

    /**
     * SVG++ event reporting the value of the attribute viewBox.
     */
    void set(svgpp::tag::attribute::viewBox /*unused*/, double x, double y,
             double width, double height) {
        Vector min{x, y};
        layout_attribs_.viewbox = Rect{min, min + Vector{width, height}};
    }

    /**
     * SVG++ event reporting the value of the attribute viewBox.
     */
    void set(svgpp::tag::attribute::preserveAspectRatio /*unused*/,
             bool /*unused*/, svgpp::tag::value::none align) {
        layout_attribs_.align = align;
    }

    /**
     * SVG++ event reporting the value of the attribute viewBox.
     */
    template <class AlignTag, class MeetOrSliceTag>
    void set(svgpp::tag::attribute::preserveAspectRatio /*unused*/,
             bool /*unused*/, AlignTag align, MeetOrSliceTag meet_or_slice) {
        layout_attribs_.align = align;
        layout_attribs_.meet_or_slice = meet_or_slice;
    }

    /**
     * SVG++ event reporting the value of the attribute patternUnits.
     */
    void set(svgpp::tag::attribute::patternUnits /*unused*/,
             svgpp::tag::value::userSpaceOnUse /*unused*/) {
        layout_attribs_.pattern_units = detail::UnitType::kUserSpaceOnUse;
    }

    /**
     * SVG++ event reporting the value of the attribute patternUnits.
     */
    void set(svgpp::tag::attribute::patternUnits /*unused*/,
             svgpp::tag::value::objectBoundingBox /*unused*/) {
        layout_attribs_.pattern_units = detail::UnitType::kObjectBoundingBox;
    }

    /**
     * SVG++ event reporting the value of the attribute patternUnits.
     */
    void set(svgpp::tag::attribute::patternContentUnits /*unused*/,
             svgpp::tag::value::userSpaceOnUse /*unused*/) {
        pattern_content_units_ = detail::UnitType::kUserSpaceOnUse;
    }

    /**
     * SVG++ event reporting the value of the attribute patternContentUnits.
     */
    void set(svgpp::tag::attribute::patternContentUnits /*unused*/,
             svgpp::tag::value::objectBoundingBox /*unused*/) {
        pattern_content_units_ = detail::UnitType::kObjectBoundingBox;
    }

    /**
     * SVG++ event fired when the element has been fully processed.
     */
    void on_exit_element();
};

template <class Exporter>
template <class ParentExporter>
PatternContext<Exporter>::PatternContext(
    ShapeContext<ParentExporter>& shape_context)
    : BaseContext<Exporter>{shape_context},
      clipping_path_{shape_context.outline_path()} {}

template <class Exporter>
bool PatternContext<Exporter>::notify(AfterViewportAttributesEvent /*unused*/) {
    // Calculate bounding box of the shape being filled
    Rect bbox;
    clipping_path_.to_polylines([&bbox](Vector start_point) {
        bbox.extend(start_point);
        return [&bbox](Vector point) { bbox.extend(point); };
    });

    auto&& result = detail::calculate_pattern_layout(
        layout_attribs_, bbox.sizes(), this->viewport());
    if (result) {
        report_transform(std::get<Transform>(*result));
        size_ = std::get<Vector>(*result);
    }

    if (pattern_content_units_ == detail::UnitType::kObjectBoundingBox) {
        report_transform(Transform{Eigen::Scaling(bbox.sizes())});
        size_ = (size_->array() / bbox.sizes().array()).matrix();
    }

    return true;
}

template <class Exporter>
void PatternContext<Exporter>::on_exit_element() {
    if (!size_) {
        return;
    }

    auto offsets =
        detail::compute_tiling_offsets(*size_, this->to_root(), clipping_path_);
    ClipperLib::PolyTree poly_tree =
        detail::clip_tiled_pattern(clipping_path_, pattern_paths_, offsets);

    ClipperLib::Paths paths;
    ClipperLib::PolyTreeToPaths(poly_tree, paths);

    for (const auto& path : paths) {
        if (path.empty()) {
            continue;
        }

        Path svg_path;
        svg_path.push_command(
            MoveCommand{detail::from_clipper_point(path.front())});
        for (std::size_t i = 1; i < path.size(); i++) {
            svg_path.push_command(
                LineCommand{detail::from_clipper_point(path[i])});
        }

        this->exporter_.plot(DashedPath{std::move(svg_path)});
    }
}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_
