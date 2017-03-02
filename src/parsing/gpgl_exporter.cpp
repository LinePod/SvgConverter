#include "gpgl_exporter.h"

/**
 * Factor to convert from millimeters to GPGL units.
 */
constexpr double kMillimeterToGpglFactor = 20;

/**
 * Convert a point from millimeter based SVG space to GPGL space.
 */
Point to_gpgl(Point point) {
    // GPGL has a x axis that points to the right and a y axis that points down
    return Point{point(1), point(0)} * kMillimeterToGpglFactor;
}

GpglExporter::GpglExporter(std::ostringstream& out_stream)
    : out_stream_{out_stream} {}

void GpglExporter::move_to(Point point) {
    point = to_gpgl(point);
    out_stream_ << "M " << point(0) << ',' << point(1) << '\x03';
}

void GpglExporter::draw_to(Point point) {
    point = to_gpgl(point);
    out_stream_ << "D " << point(0) << ',' << point(1) << '\x03';
}
