#include "graphical_element.h"

const CoordinateSystem& GraphicsElementContext::coordinate_system() const {
    return coordinate_system_;
}

void GraphicsElementContext::transform_matrix(
        const boost::array<double, 6>& matrix) {
    Transform transform;
    transform.matrix() << matrix[0], matrix[2], matrix[4],
                          matrix[1], matrix[3], matrix[5];
    coordinate_system_ = CoordinateSystem{coordinate_system_, transform};
}

const LengthFactory& GraphicsElementContext::length_factory() const {
    return viewport_.length_factory();
}
