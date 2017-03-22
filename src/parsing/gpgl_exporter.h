#ifndef SVG_CONVERTER_GPGL_EXPORTER_H
#define SVG_CONVERTER_GPGL_EXPORTER_H

#include <sstream>

#include "../math_defs.h"

class GpglExporter {
 private:
    std::ostringstream& out_stream_;

 public:
    /**
     * Creates a new gpgl exporter writing to the given stream.
     *
     * The reference must be valid for the lifetime of the exporter and all its
     * copies.
     */
    explicit GpglExporter(std::ostringstream& out_stream);

    GpglExporter(const GpglExporter&) = default;

    GpglExporter& operator=(const GpglExporter&) = default;

    void move_to(Point point);

    void plot_to(Point point);
};

#endif  // SVG_CONVERTER_GPGL_EXPORTER_H
