#ifndef SVG_CONVERTER_COORDINATE_SYSTEM_H
#define SVG_CONVERTER_COORDINATE_SYSTEM_H

#include <eigen3/Eigen/Geometry>

/**
 * An SVG affine transformation.
 */
using Transform = Eigen::AffineCompact2d;

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
};

#endif  // SVG_CONVERTER_COORDINATE_SYSTEM_H
