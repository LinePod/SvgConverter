#include "gpgl_exporter.h"

/**
 * Factor to convert from millimeters to GPGL units.
 */
constexpr double kMillimeterToGpglFactor = 20;

/**
 * Convert a point from millimeter based SVG space to GPGL space.
 */
Vector to_gpgl(Vector point) {
    // GPGL has a x axis that points to the right and a y axis that points down
    return Vector{point(1), point(0)} * kMillimeterToGpglFactor;
}

GpglExporter::GpglExporter(std::ostringstream& out_stream)
    : out_stream_{out_stream} {}

void GpglExporter::plot(const Path& path,
                        const std::vector<double>& dasharray) {
    path.to_polylines(dasharray, [this](Vector start_point) {
        start_point = to_gpgl(start_point);
        out_stream_ << "M " << start_point(0) << ',' << start_point(1)
                    << '\x03';
        return [this](Vector point) {
            point = to_gpgl(point);
            out_stream_ << "D " << point(0) << ',' << point(1) << '\x03';
        };
    });
}
