#ifndef SVG_CONVERTER_VIEWPORT_H
#define SVG_CONVERTER_VIEWPORT_H

#include <svgpp/factory/unitless_length.hpp>

/**
 * Describes the mapping between user units and absolute units like millimeters.
 *
 * We use 20 so that the user units match the GPGL units at the top level.
 */
constexpr double user_units_per_mm = 20;

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
     * Creates a new viewport with default sizes.
     */
    Viewport();

    /**
     * Sets the size of the viewport.
     *
     * @param width Width of the viewport in the surrounding viewports units.
     * @param height Height of the viewport in the surrounding viewports units.
     */
    void set_size(double width, double height);

    /**
     * Svg++ length factory for elements inside this viewport.
     */
    const LengthFactory& length_factory() const;
};

#endif  // SVG_CONVERTER_VIEWPORT_H
