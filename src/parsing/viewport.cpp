#include "viewport.h"

Viewport::Viewport() {
    length_factory_.set_absolute_units_coefficient(
            user_units_per_mm, svgpp::tag::length_units::mm{});
}

void Viewport::set_size(double width, double height) {
    length_factory_.set_viewport_size(width, height);
}

const LengthFactory& Viewport::length_factory() const {
    return length_factory_;
}
