#include "viewport.h"

Viewport::Viewport(double print_area_width, double print_area_height) {
    length_factory_.set_viewport_size(print_area_width, print_area_height);
    length_factory_.set_absolute_units_coefficient(
        kUserUnitsPerMillimeter, svgpp::tag::length_units::mm{});
}

void Viewport::set_size(double width, double height) {
    length_factory_.set_viewport_size(width, height);
}

const LengthFactory& Viewport::length_factory() const {
    return length_factory_;
}
