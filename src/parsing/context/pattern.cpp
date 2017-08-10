#include "pattern.h"

#include <boost/variant.hpp>
#include <eigen3/Eigen/SVD>

#include <cstdint>
#include <utility>

/**
 * Scale factor applied before rounding to integer coordinates for clipping.
 *
 * Can be adjusted to avoid artifacts created by unexact clipping.
 */
constexpr double kClipperAccuracyFactor = 10;

ClipperLib::IntPoint to_clipper_point(const Vector& point) {
    auto int_point = (point * kClipperAccuracyFactor)
                         .array()
                         .round()
                         .cast<ClipperLib::cInt>();
    return {int_point(0), int_point(1)};
}

/**
 * Polyline visitor created from two lambdas.
 *
 * The first lambda gets called for each point, the second at the end of the
 * polyline.
 *
 * Because the visitor needs to be able to be moved (to be returned from the
 * factory function), this implementation has to track whether an instance has
 * been moved. If it has been moved, we don't call the on end function in the
 * destructor.
 */
// TODO(David): Maybe refactor Path::to_polylines to not use the destructor?
template <class PointFunc, class OnEndFunc>
class DualLambdaPolylineVisitor {
 private:
    PointFunc point_func_;
    OnEndFunc on_end_func_;

    /**
     * Needed to save whether this visitor was moved out of.
     */
    bool is_alive = true;

 public:
    DualLambdaPolylineVisitor(PointFunc point_func, OnEndFunc on_end_func)
        : point_func_{point_func}, on_end_func_{on_end_func} {}

    // Make move only
    DualLambdaPolylineVisitor(const DualLambdaPolylineVisitor&) = delete;
    DualLambdaPolylineVisitor& operator=(const DualLambdaPolylineVisitor&) =
        delete;

    DualLambdaPolylineVisitor(DualLambdaPolylineVisitor&& other) noexcept
        : point_func_{other.point_func_}, on_end_func_{other.on_end_func_} {
        other.is_alive = false;
    }

    DualLambdaPolylineVisitor& operator=(
        DualLambdaPolylineVisitor&& other) noexcept {
        point_func_ = other.point_func_;
        on_end_func_ = other.on_end_func_;
        is_alive = true;
        other.is_alive = false;
        return *this;
    }

    ~DualLambdaPolylineVisitor() {
        if (is_alive) {
            on_end_func_();
        }
    }

    void operator()(Vector point) { point_func_(point); }
};

/**
 * Visitor that calculates the correct viewbox from attributes.
 *
 * Basically a copy of `collect_viewbox_adapter::options_visitor` from
 * svgpp/adapter/viewport.hpp.
 */
class ViewboxVisitor
    : public boost::static_visitor<std::tuple<Vector, Vector>> {
 private:
    const Rect& viewbox_;
    double viewport_width_;
    double viewport_height_;

 public:
    ViewboxVisitor(const Rect& viewbox, double viewport_width,
                   double viewport_height)
        : viewbox_(viewbox),
          viewport_width_(viewport_width),
          viewport_height_(viewport_height) {}

    template <class AlignTag, class MeetOrSliceTag>
    std::tuple<Vector, Vector> operator()(
        const AlignTag& align_tag,
        const MeetOrSliceTag& meet_or_slice_tag) const {
        Vector translate;
        Vector scale;
        Vector viewbox_pos = viewbox_.min();
        Vector viewbox_size = viewbox_.sizes();
        svgpp::calculate_viewbox_transform<double>::calculate(
            viewport_width_, viewport_height_, viewbox_pos.x(), viewbox_pos.y(),
            viewbox_size.x(), viewbox_size.y(), align_tag, meet_or_slice_tag,
            translate.x(), translate.y(), scale.x(), scale.y());
        return std::make_tuple(translate, scale);
    }
};

/**
 * Creates a `DualLambdaPolylineVisitor` with perfect forwarding.
 */
template <class PointFunc, class OnEndFunc>
DualLambdaPolylineVisitor<PointFunc, OnEndFunc> make_dual_lambda_visitor(
    PointFunc&& point_func, OnEndFunc&& on_end_func) {
    return {std::forward<PointFunc>(point_func),
            std::forward<OnEndFunc>(on_end_func)};
}

boost::optional<std::tuple<Transform, Vector>> detail::calculate_pattern_layout(
    const PatternLayoutAttributes& attribs, Vector bbox_size,
    const Viewport& viewport) {
// This code is very closely modeled after SVG++s own code in
// svgpp/adapter/viewport.hpp.

// We keep the float comparisons to stay equivalent to SVG++'s code
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    namespace length_units = svgpp::tag::length_units;
    namespace length_dimension = svgpp::tag::length_dimension;

    auto&& length_factory = viewport.length_factory();
    Transform transform = Transform::Identity();
    Vector size{0, 0};

    double default_x = length_factory.create_length(0, length_units::none{});
    double default_y = length_factory.create_length(0, length_units::none{});
    double default_width = length_factory.create_length(
        100, length_units::percent{}, length_dimension::width{});
    double default_height = length_factory.create_length(
        100, length_units::percent{}, length_dimension::height{});

    double viewport_x = attribs.x.value_or(default_x);
    double viewport_y = attribs.y.value_or(default_y);
    double viewport_width = attribs.width.value_or(default_width);
    double viewport_height = attribs.height.value_or(default_height);

    if (viewport_width < 0 || viewport_height < 0) {
        // TODO(David): Handle using normal SVG++ error handling
        return boost::none;
    }

    if (viewport_width == 0 || viewport_height == 0) {
        return boost::none;
    }

    // Added handling for patternUnits
    if (attribs.pattern_units == UnitType::kObjectBoundingBox) {
        viewport_x *= bbox_size.x();
        viewport_y *= bbox_size.y();
        viewport_width *= bbox_size.x();
        viewport_height *= bbox_size.y();
    }

    transform.pretranslate(Vector{viewport_x, viewport_y});

    if (attribs.viewbox) {
        auto&& viewbox = *attribs.viewbox;
        Vector viewbox_size = viewbox.sizes();
        if (viewbox_size.x() < 0 || viewbox_size.y() < 0) {
            // TODO(David): Handle using normal SVG++ error handling
            return boost::none;
        }

        if (viewbox_size.x() == 0 || viewbox_size.y() == 0) {
            return boost::none;
        }

        Vector translate;
        Vector scale;
        std::tie(translate, scale) = boost::apply_visitor(
            ViewboxVisitor{viewbox, viewport_width, viewport_height},
            attribs.align, attribs.meet_or_slice);
        transform.pretranslate(translate).prescale(scale);
        viewport_width = viewbox_size.x();
        viewport_height = viewbox_size.y();
    }

    return std::make_tuple(transform, Vector{viewport_width, viewport_height});
#pragma clang diagnostic pop
}

std::vector<Vector> detail::compute_tiling_offsets(const Vector& pattern_size,
                                                   const Transform& to_root,
                                                   const Path& clipping_path) {
    // We use a very simple approach to tiling here: In the coordinate system
    // it is defined in, the pattern is a rectangle located at (0, 0). We add an
    // additional scale, so that the size of the pattern is (1, 1). Then we take
    // the inverse of that and transform our clipping path into this coordinate
    // system. We build the bounding box of the clipping path and round it
    // outward to the nearest integer coordinates. Now we can just enumerate
    // all integer coordinates in the bounding box and transform them into
    // root space.
    //
    // An alternative approach that would not require inverting a matrix would
    // be to use the fact, that the affine transformation to root space can
    // only transform the rectangle into a parallelogram, and than doing
    // parallelogram tiling in root space.

    // Transforms from the unit coordinate system (were one instance of the
    // pattern is (1, 1) in size) to and from the root coordinate system.
    Transform unit_to_root = to_root;
    unit_to_root.scale(pattern_size);
    Transform unit_from_root =
        unit_to_root.inverse(Eigen::TransformTraits::AffineCompact);

    Rect bounding_box;
    clipping_path.to_polylines(
        [&bounding_box, &unit_from_root](Vector start_point) {
            bounding_box.extend(unit_from_root * start_point);
            return [&bounding_box, &unit_from_root](Vector point) {
                bounding_box.extend(unit_from_root * point);
            };
        });

    std::vector<Vector> result;
    Vector base_point = unit_to_root * Vector{0, 0};

    // int64_t can hold all reasonable values that the double coefficients can
    // have
    // Note to self: Don't cast to int to perform a floor operation, if your
    // values can be negative.
    auto int_min_point =
        bounding_box.min().array().floor().matrix().cast<std::int64_t>();
    auto int_max_point =
        bounding_box.max().array().floor().matrix().cast<std::int64_t>();

    for (std::int64_t x = int_min_point(0); x <= int_max_point(0); x++) {
        for (std::int64_t y = int_min_point(1); y <= int_max_point(1); y++) {
            result.emplace_back(unit_to_root * Vector{x, y} - base_point);
        }
    }

    return result;
}

ClipperLib::PolyTree detail::clip_tiled_pattern(
    const Path& clipping_path, const std::vector<DashedPath>& pattern_paths,
    const std::vector<Vector>& offsets) {
    // We reuse the same path for all paths added to the clipper instance to
    // save on memory allocation
    ClipperLib::Path clipper_path;
    ClipperLib::Clipper clipper;

    clipping_path.to_polylines([&](Vector start_point) {
        clipper_path.push_back(to_clipper_point(start_point));
        return make_dual_lambda_visitor(
            [&](Vector point) {
                clipper_path.push_back(to_clipper_point(point));
            },
            [&]() {
                clipper.AddPath(clipper_path, ClipperLib::PolyType::ptClip,
                                true);
                clipper_path.clear();
            });
    });

    // Reused like `clipper_path`
    std::vector<Vector> polyline;
    for (const auto& dashed_path : pattern_paths) {
        dashed_path.to_polylines([&](Vector start_point) {
            polyline.push_back(start_point);
            return make_dual_lambda_visitor(
                [&](Vector point) { polyline.push_back(point); },
                [&]() {
                    for (const Vector& offset : offsets) {
                        for (const Vector& point : polyline) {
                            clipper_path.push_back(
                                to_clipper_point(point + offset));
                        }

                        clipper.AddPath(clipper_path,
                                        ClipperLib::PolyType::ptSubject, false);
                        clipper_path.clear();
                    }

                    polyline.clear();
                });
        });
    }

    ClipperLib::PolyTree result;
    clipper.Execute(ClipperLib::ClipType::ctIntersection, result);
    return result;
}

Vector detail::from_clipper_point(ClipperLib::IntPoint point) {
    return Vector{point.X, point.Y} / kClipperAccuracyFactor;
}

detail::PatternExporter::PatternExporter(std::vector<DashedPath>& paths)
    : paths_{paths} {}

void detail::PatternExporter::plot(DashedPath path) {
    paths_.emplace_back(std::move(path));
}
