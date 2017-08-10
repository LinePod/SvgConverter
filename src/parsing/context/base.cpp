#include "base.h"

detail::BaseContextExporterless::BaseContextExporterless(
    const SvgDocument& document, spdlog::logger& logger,
    const Viewport& viewport, const Transform& to_root)
    : to_root_{to_root},
      document_{document},
      logger_{logger},
      viewport_{viewport} {}

void detail::BaseContextExporterless::transform_matrix(
    const boost::array<double, 6>& matrix) {
    Transform transform;
    // clang-format off
    transform.matrix() << matrix[0], matrix[2], matrix[4],
                          matrix[1], matrix[3], matrix[5];
    // clang-format on
    to_root_ = to_root_ * transform;
}
