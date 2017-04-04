#include "pattern.h"

#include "../dashes.h"

detail::PatternExporter::PatternExporter(
    std::vector<std::vector<Vector>>& paths)
    : paths_{paths} {}

void detail::PatternExporter::plot(const std::vector<Vector>& polyline,
                                   const std::vector<double>& dasharray) {
    // Generate fewer lines in case the line is not dashed (to_dashes would
    // generate a line per line segment)
    if (dasharray.empty()) {
        paths_.push_back(polyline);
        return;
    }

    to_dashes(polyline, dasharray, [this](Vector start, Vector end) {
        paths_.push_back({start, end});
    });
}
