#ifndef SVG_CONVERTER_PARSING_VIEWPORT_H_
#define SVG_CONVERTER_PARSING_VIEWPORT_H_

#include <svgpp/factory/unitless_length.hpp>

/**
 * Describes the mapping between user units and absolute units like millimeters.
 *
 * We use 1 so that the user units on the top level of the svg are equal to
 * millimeters, which is more intuitive than for example the 1/20mm GPGL units.
 */
constexpr double kUserUnitsPerMillimeter = 1;

using LengthFactory = svgpp::factory::length::unitless<>;

/**
 * SVG viewport, consisting of a size and a unit to user units mapping.
 *
 * See https://www.w3.org/TR/SVG/coords.html for SVGs handling of coordinate
 * systems and viewports.
 */
class Viewport {
 private:
    LengthFactory length_factory_;

 public:
    /**
     * Creates a viewport with the given size.
     */
    Viewport(double width, double height);

    /**
     * SVG++ length factory for elements inside this viewport.
     */
    const LengthFactory& length_factory() const;
};

#endif  // SVG_CONVERTER_PARSING_VIEWPORT_H_
