#include "coordinate_system.h"

CoordinateSystem::CoordinateSystem() : transform_{Transform::Identity()} {}

CoordinateSystem::CoordinateSystem(const CoordinateSystem& parent,
                                   const Transform& transform)
    : transform_{parent.transform_ * transform} {}

Vector CoordinateSystem::to_root(Vector point) const {
    return transform_ * point;
}
