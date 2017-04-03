#include "gpgl_exporter.h"

#include "dashes.h"

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

void GpglExporter::plot(const std::vector<Point>& polyline,
                        const std::vector<double>& dasharray) {
    to_dashes(polyline, dasharray, [this](Point start, Point end) {
        start = to_gpgl(start);
        end = to_gpgl(end);
        out_stream_ << "M " << start(0) << ',' << start(1) << '\x03';
        out_stream_ << "D " << end(0) << ',' << end(1) << '\x03';
    });
}
