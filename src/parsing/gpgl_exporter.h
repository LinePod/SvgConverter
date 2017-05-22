#ifndef SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
#define SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_

#include <functional>
#include <sstream>

#include "../math_defs.h"
#include "dashes.h"

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
     * Export the given dashed path.
     */
    void plot(const DashedPath& path);
};

#endif  // SVG_CONVERTER_PARSING_GPGL_EXPORTER_H_
