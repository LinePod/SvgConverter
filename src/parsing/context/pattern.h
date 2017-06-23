#ifndef SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_
#define SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_

#include <vector>

#include <clipper.hpp>

#include "../../math_defs.h"
#include "../dashes.h"
#include "../path.h"
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
 * Generate a tiling for a pattern to completely fill the given clipping path.
 *
 * @param pattern_size Size of the pattern rectangle in the coordinate system it
 *                     is defined in.
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
     * Size of the pattern as set by `width` and `height` or by `viewBox`.
     *
     * `none` if rendering was disabled via a width/height of 0.
     */
    boost::optional<Vector> size_ = boost::none;

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
     * Whether child elements should be processed.
     */
    bool process_children() const { return size_ != boost::none; }

    /**
     * SVG++ event reporting x, y, width and height properties.
     */
    void set_viewport(double /*unused*/, double /*unused*/, double width,
                      double height) {
        // Scaling and translation due to x, y, width, height and viewBox is
        // handled by SVG++ due to the viewport policy `as_transform`
        size_ = Vector{width, height};
    }

    /**
     * SVG++ event reporting the viewbox size set by the `viewbox` attribute.
     */
    void set_viewbox_size(double width, double height) {
        size_ = Vector{width, height};
    }

    /**
     * SVG++ event reporting viewport width and/or height being set to 0.
     *
     * The SVG spec defines that the content rendering should be disabled in
     * this case.
     */
    void disable_rendering() { size_ = boost::none; }

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
