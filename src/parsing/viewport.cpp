#include "viewport.h"

Viewport::Viewport(double width, double height) {
    length_factory_.set_viewport_size(width, height);
    length_factory_.set_absolute_units_coefficient(
        kUserUnitsPerMillimeter, svgpp::tag::length_units::mm{});
}

const LengthFactory& Viewport::length_factory() const {
    return length_factory_;
}
