#include "dashes.h"

DashedPath::DashedPath(Path path, std::vector<double> dasharray,
                       const Transform& to_local)
    : path_{std::move(path)},
      dasharray_{std::move(dasharray)},
      to_local_{to_local} {}

DashedPath::DashedPath(Path path)
    : path_{std::move(path)}, to_local_{Transform::Identity()} {}
