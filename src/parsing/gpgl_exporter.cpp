#include "gpgl_exporter.h"

#include <iomanip>

/**
 * Factor to convert from millimeters to GPGL units.
 */
constexpr double kMillimeterToGpglFactor = 20;

/**
 * Convert a point from millimeter based SVG space to GPGL space.
 *
 * Also rounds to a reasonable export precision.
 */
Vector to_gpgl(Vector point) {
    // GPGL has a x axis that points to the right and a y axis that points down
    Vector gpgl = Vector{point(1), point(0)} * kMillimeterToGpglFactor;
    return gpgl.array().round().matrix();
}

GpglExporter::GpglExporter(std::ostringstream& out_stream)
    : out_stream_{out_stream} {
    // Print double values without any decimal places.
    // This works in tandem with the rounding done in `to_gpgl`.
    out_stream_.get() << std::setprecision(0) << std::fixed;
}

void GpglExporter::plot(const Path& path,
                        const std::vector<double>& dasharray) {
    path.to_polylines(dasharray, [this](Vector start_point) {
        start_point = to_gpgl(start_point);
        out_stream_.get() << "M " << start_point(0) << ',' << start_point(1)
                          << '\x03';
        return [this](Vector point) {
            point = to_gpgl(point);
            out_stream_.get() << "D " << point(0) << ',' << point(1) << '\x03';
        };
    });
}
