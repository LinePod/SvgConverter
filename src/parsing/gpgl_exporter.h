#ifndef SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
#define SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_

#include <functional>
#include <sstream>

#include "../math_defs.h"
#include "path.h"

class GpglExporter {
 private:
    // Reference wrapper to make the reference copyable.
    std::reference_wrapper<std::ostringstream> out_stream_;

 public:
    /**
     * Creates a new gpgl exporter writing to the given stream.
     *
     * The reference must be valid for the lifetime of the exporter and all its
     * copies.
     */
    explicit GpglExporter(std::ostringstream& out_stream);

    /**
     * Export the given path using the given dasharray.
     *
     * If the dasharray is empty, the lines are drawn fully solid.
     */
    void plot(const Path& path, const std::vector<double>& dasharray);
};

#endif  // SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
