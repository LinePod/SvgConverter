#ifndef SVG_CONVERTER_PARSING_COORDINATE_SYSTEM_H_
#define SVG_CONVERTER_PARSING_COORDINATE_SYSTEM_H_

#include "../math_defs.h"

/**
 * An SVG coordinate system.
 *
 * See https://www.w3.org/TR/SVG/coords.html for SVGs handling of coordinate
 * systems and viewports.
 */
class CoordinateSystem {
 private:
    /**
     * Cumulative transform from this space to the root coordinate system.
     */
    Transform transform_;

 public:
    /**
     * Creates a root coordinate system.
     */
    CoordinateSystem();

    /**
     * Creates a new derived coordinate system.
     *
     * @param parent Coordinate system to derive from.
     * @param transform Transform from the new coordinate system to the parent.
     */
    CoordinateSystem(const CoordinateSystem& parent,
                     const Transform& transform);

    /**
     * Transforms a point from this coordinate system to the root one.
     */
    Vector to_root(Vector point) const;

    /**
     * Accesses a transform from this coordinate system to the root one.
     */
    const Transform& transform() const;
};

#endif  // SVG_CONVERTER_PARSING_COORDINATE_SYSTEM_H_
