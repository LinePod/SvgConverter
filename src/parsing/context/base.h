#ifndef SVG_CONVERTER_BASE_H
#define SVG_CONVERTER_BASE_H

#include "../gpgl_exporter.h"

/**
 * Base class for all context classes.
 */
class BaseContext {
 private:
    /**
     * Exporter for generated shapes.
     *
     * Stored here because it needs to be passed down through the hierarchy of
     * contexts.
     */
    GpglExporter exporter_;

 protected:
    explicit BaseContext(GpglExporter exporter) : exporter_{exporter} {}

    explicit BaseContext(const BaseContext& parent)
        : exporter_{parent.exporter_} {}

    /**
     * Accesses the stored exporter.
     */
    GpglExporter& exporter() { return exporter_; }
};

#endif  // SVG_CONVERTER_BASE_H
