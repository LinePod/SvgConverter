#include "viewport.h"

Viewport::Viewport(double width, double height) {
    length_factory_.set_viewport_size(width, height);
    length_factory_.set_absolute_units_coefficient(
        kUserUnitsPerMillimeter, svgpp::tag::length_units::mm{});
}

const LengthFactory& Viewport::length_factory() const {
    return length_factory_;
}

Vector Viewport::size() const {
    // Avoids double saving the width and height
    double width =
        length_factory_.create_length(100, svgpp::tag::length_units::percent{},
                                      svgpp::tag::length_dimension::width{});
    double height =
        length_factory_.create_length(100, svgpp::tag::length_units::percent{},
                                      svgpp::tag::length_dimension::height{});
    return {width, height};
}
