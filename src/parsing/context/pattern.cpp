#include "pattern.h"

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
 * Creates a `DualLambdaPolylineVisitor` with perfect forwarding.
 */
template <class PointFunc, class OnEndFunc>
DualLambdaPolylineVisitor<PointFunc, OnEndFunc> make_dual_lambda_visitor(
    PointFunc&& point_func, OnEndFunc&& on_end_func) {
    return {std::forward<PointFunc>(point_func),
            std::forward<OnEndFunc>(on_end_func)};
}

std::vector<Vector> detail::compute_tiling_offsets(
    const Vector& pattern_size, const CoordinateSystem& coordinate_system,
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

    Transform to_root = coordinate_system.transform();
    to_root.scale(pattern_size);
    Transform from_root =
        to_root.inverse(Eigen::TransformTraits::AffineCompact);

    Rect bounding_box;
    clipping_path.to_polylines(
        {}, [&bounding_box, &from_root](Vector start_point) {
            bounding_box.extend(from_root * start_point);
            return [&bounding_box, &from_root](Vector point) {
                bounding_box.extend(from_root * point);
            };
        });

    std::vector<Vector> result;
    Vector base_point = to_root * Vector{0, 0};

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
            result.emplace_back(to_root * Vector{x, y} - base_point);
        }
    }

    return result;
}

ClipperLib::PolyTree detail::clip_tiled_pattern(
    const Path& clipping_path,
    const std::vector<detail::DashedPath>& pattern_paths,
    const std::vector<Vector>& offsets) {
    // We reuse the same path for all paths added to the clipper instance to
    // save on memory allocation
    ClipperLib::Path clipper_path;
    ClipperLib::Clipper clipper;

    clipping_path.to_polylines({}, [&](Vector start_point) {
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
        dashed_path.path.to_polylines(
            dashed_path.dasharray, [&](Vector start_point) {
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
                                            ClipperLib::PolyType::ptSubject,
                                            false);
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

void detail::PatternExporter::plot(Path path, std::vector<double> dasharray) {
    paths_.push_back({std::move(path), std::move(dasharray)});
}
