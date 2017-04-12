#include "coordinate_system.h"

CoordinateSystem::CoordinateSystem() : transform_{Transform::Identity()} {}

CoordinateSystem::CoordinateSystem(const CoordinateSystem& parent,
                                   const Transform& transform)
    : transform_{parent.transform_ * transform} {}

const Transform& CoordinateSystem::transform() const { return transform_; }
