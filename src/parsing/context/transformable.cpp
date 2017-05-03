#include "transformable.h"

TransformableContext::TransformableContext(CoordinateSystem coordinate_system)
    : coordinate_system_{std::move(coordinate_system)} {}

const CoordinateSystem& TransformableContext::coordinate_system() const {
    return coordinate_system_;
}

void TransformableContext::transform_matrix(
    const boost::array<double, 6>& matrix) {
    Transform transform;
    // clang-format off
    transform.matrix() << matrix[0], matrix[2], matrix[4],
                          matrix[1], matrix[3], matrix[5];
    // clang-format on
    coordinate_system_ = CoordinateSystem{coordinate_system_, transform};
}
