#include "pattern.h"

#include "../dashes.h"

detail::PatternExporter::PatternExporter(std::vector<std::vector<Point>>& paths)
    : paths_{paths} {}

void detail::PatternExporter::plot(const std::vector<Point>& polyline,
                                   const std::vector<double>& dasharray) {
    // Generate fewer lines in case the line is not dashed (to_dashes would
    // generate a line per line segment)
    if (dasharray.empty()) {
        paths_.push_back(polyline);
        return;
    }

    to_dashes(polyline, dasharray, [this](Point start, Point end) {
        paths_.push_back({start, end});
    });
}
