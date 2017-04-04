#ifndef SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_
#define SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_

#include <algorithm>
#include <vector>

#include "../../math_defs.h"
#include "../coordinate_system.h"
#include "../viewport.h"
#include "base.h"
#include "transformable.h"
#include "viewport_establishing.h"

namespace detail {

/**
 * Exports shapes to a list of paths, which can than be tiled.
 */
class PatternExporter {
 private:
    std::vector<std::vector<Vector>>& paths_;

 public:
    /**
     * Create a new exporter.
     *
     * @param paths List of paths to write to. Reference must be valid for the
     *              lifetime of the exporter and its copies.
     */
    explicit PatternExporter(std::vector<std::vector<Vector>>& paths);

    /**
     * Export the given polyline using the given dasharray.
     *
     * If the dasharray is empty, the lines are drawn fully solid.
     */
    void plot(const std::vector<Vector>& polyline,
              const std::vector<double>& dasharray);
};

/**
 * Generate a tiling for a pattern to completely fill the given clipping path.
 *
 * @param pattern_size Size of the pattern rectangle in the coordinate system it
 *                     is defined in.
 * @param coordinate_system Coordinate system the pattern is defined in.
 * @param clipping_path Path in global coordinates that should be completely
 *                      tiled.
 * @return List of offsets, so that when the pattern is repeated at all these
 *         offsets it will completely cover the given clipping path. If a
 *         pattern with no offset is needed (offset of (0, 0)), it will be
 *         included.
 */
// TODO(David): Change this to not allocate for the list of points
std::vector<Vector> compute_tiling_offsets(
    Vector pattern_size, const CoordinateSystem& coordinate_system,
    const std::vector<std::vector<Vector>>& clipping_path);

}  // namespace detail

/**
 * Pseudo context used to pass needed data to a `PatternContext`.
 *
 * This context should be created in a `ShapeContext`, filled with the needed
 * data and passed to `load_referencing_element`. SVG++ will then create a
 * `PatternContext` which will get its data from the pseudo context.
 *
 * Named pseudo context because it is not associated with an element and only
 * used to forward data.
 *
 * Still implements `BaseContext` to avoid special cases in some instances.
 */
template <class Exporter>
class PatternPseudoContext : public BaseContext {
 public:
    PatternPseudoContext(const BaseContext& parent, Exporter exporter,
                         const Viewport& shape_viewport,
                         const CoordinateSystem& shape_coordinate_system,
                         const std::vector<std::vector<Vector>>& clipping_path);

    /**
     * Exporter to export the tiled pattern with.
     */
    Exporter exporter_;

    /**
     * Viewport of the shape being filled with the pattern.
     */
    const Viewport& shape_viewport_;

    /**
     * Coordinate system of the shape being filled with the pattern.
     */
    const CoordinateSystem& shape_coordinate_system_;

    /**
     * Outline to fill, in global coordinates.
     *
     * Each subvector describes a single closed subpath.
     */
    const std::vector<std::vector<Vector>>& clipping_path_;
};

/**
 * Context for <pattern> elements.
 *
 * This context will be created by SVG++ when passing a `PatternPseudoContext`
 * and a <pattern> element to `load_referencing_element`.
 *
 * It needs shape specific information (like the current viewport and coordinate
 * system as well as the area being tiled with the pattern). If multiple shapes
 * use the same pattern, it is parsed once for each shape. Also, if <path>
 * contains multiple polygons, the pattern will be parsed once for each polygon.
 */
template <class Exporter>
class PatternContext : public BaseContext,
                       public TransformableContext,
                       public ViewportEstablishingContext {
 private:
    Exporter exporter_;

    const Viewport& shape_viewport_;

    const std::vector<std::vector<Vector>>& clipping_path_;

    /**
     * Gets filled with a single instance of the pattern via `PatternExporter`.
     */
    std::vector<std::vector<Vector>> pattern_lines_;

 public:
    explicit PatternContext(
        const PatternPseudoContext<Exporter>& pseudo_parent);

    const LengthFactory& length_factory() const;

    const CoordinateSystem& inner_coordinate_system() const;

    detail::PatternExporter inner_exporter();

    void on_exit_element();
};

template <class Exporter>
PatternPseudoContext<Exporter>::PatternPseudoContext(
    const BaseContext& parent, Exporter exporter,
    const Viewport& shape_viewport,
    const CoordinateSystem& shape_coordinate_system,
    const std::vector<std::vector<Vector>>& clipping_path)
    : BaseContext(parent),
      exporter_(exporter),
      shape_viewport_(shape_viewport),
      shape_coordinate_system_(shape_coordinate_system),
      clipping_path_(clipping_path) {}

template <class Exporter>
PatternContext<Exporter>::PatternContext(
    const PatternPseudoContext<Exporter>& pseudo_parent)
    : BaseContext{pseudo_parent},
      TransformableContext{pseudo_parent.shape_coordinate_system_},
      // Pattern viewports have a default size of 0 x 0
      ViewportEstablishingContext{boost::none},
      exporter_{pseudo_parent.exporter_},
      shape_viewport_{pseudo_parent.shape_viewport_},
      clipping_path_{pseudo_parent.clipping_path_} {}

template <class Exporter>
const LengthFactory& PatternContext<Exporter>::length_factory() const {
    // Used for the attributes width/height/x/y, which should be resolved in the
    // referencing elements viewport (for things like percentages).
    return shape_viewport_.length_factory();
}

template <class Exporter>
detail::PatternExporter PatternContext<Exporter>::inner_exporter() {
    return detail::PatternExporter{pattern_lines_};
}

template <class Exporter>
const CoordinateSystem& PatternContext<Exporter>::inner_coordinate_system()
    const {
    // We want the contained shapes to get exported in global coordinates,
    // therefore we return the full coordinate system including the transforms
    // from the referencing shape element.
    return coordinate_system();
}

template <class Exporter>
void PatternContext<Exporter>::on_exit_element() {
    auto& pattern_viewport = inner_viewport();
    Vector pattern_size{pattern_viewport.width(), pattern_viewport.height()};
    auto offsets = detail::compute_tiling_offsets(
        pattern_size, coordinate_system(), clipping_path_);
    for (auto& line : pattern_lines_) {
        std::vector<Vector> line_with_offset{line.size()};
        for (auto offset : offsets) {
            std::transform(line.begin(), line.end(), line_with_offset.begin(),
                           [offset](Vector point) { return point + offset; });
            exporter_.plot(line_with_offset, {});
        }
    }
}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_PATTERN_H_
