#ifndef SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
#define SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_

#include <sstream>
#include <vector>

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

    /**
     * Export the given polyline using the given dasharray.
     *
     * If the dasharray is empty, the lines are drawn fully solid.
     */
    void plot(const std::vector<Point>& polyline,
              const std::vector<double>& dasharray);
};

#endif  // SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
