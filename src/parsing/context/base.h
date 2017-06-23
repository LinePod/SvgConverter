#ifndef SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_BASE_H_

// clang-tidy complains no matter how these two are ordered
#include <spdlog/spdlog.h>  // NOLINT
#include <boost/array.hpp>  // NOLINT

#include "../../math_defs.h"
#include "../../svg.h"
#include "../viewport.h"

namespace detail {

/**
 * Provides all the non exporter related functionality of `BaseContext`
 *
 * Used to reduce instantiations and allow some functions to be defined in
 * `base.cpp`.
 */
class BaseContextExporterless {
 private:
    /**
     * Transformation from the local coordinate system to the root one.
     *
     * The root coordinate system is one where each unit is a millimeter
     * and the (0, 0) point is on the top left of the paper.
     */
    Transform to_root_;

    /**
     * Reference to the document being parsed.
     *
     * Used to lookup referenced elements.
     */
    const SvgDocument& document_;

    /**
     * Logger for all conversion related messages.
     */
    spdlog::logger& logger_;

    /**
     * Viewport to which this element belongs.
     *
     * Established by the next <svg> ancestor. Equal to the global
     * viewport for the outermost <svg> element.
     */
    const Viewport& viewport_;

 protected:
    BaseContextExporterless(const SvgDocument& document, spdlog::logger& logger,
                            const Viewport& viewport, const Transform& to_root);

 public:
    /**
     * SVG document the element being parsed belongs to.
     */
    const SvgDocument& document() const { return document_; }

    /**
     * Logger to use for all conversion related messages.
     */
    spdlog::logger& logger() { return logger_; }

    /**
     * Viewport the element is part of.
     */
    const Viewport& viewport() const { return viewport_; }

    /**
     * Transform to the root coordinate system.
     */
    const Transform& to_root() const { return to_root_; }

    /**
     * Handle a transform being reported by SVG++.
     */
    void transform_matrix(const boost::array<double, 6>& matrix);

    /**
     * Provides a length factory for SVG++ to resolve units.
     *
     * Based on the viewport the element is in.
     */
    const LengthFactory& length_factory() const {
        return viewport_.length_factory();
    }
};

}  // namespace detail

/**
 * Base context class.
 *
 * Provides:
 *  - Handling of viewports and coordinate systems. This is needed even for
 *    contexts related to patterns, because they are reparsed for every shape
 *    that references them and need to know the referencing shapes viewport and
 *    coordinate system to produce a output in global coordinates.
 *  - Handling of an exporter, with which generated lines should be exported.
 *  - Access to the SVG document to find referenced elements like patterns
 *  - Logging
 *  - Functionality to disable processing of child elements dynamically. All
 *    derived classes must define a constant `process_children` method, that
 *    returns a boolean indicating whether children elements should be
 *    processed.
 *  - A protocol to create contexts for child elements that facilitates all of
 *    the above. See the public constructor for more details.
 */
template <class Exporter>
class BaseContext : public detail::BaseContextExporterless {
 protected:
    /**
     * Exporter used to report generated lines.
     */
    Exporter exporter_;

    BaseContext(const SvgDocument& document, spdlog::logger& logger,
                Exporter exporter, const Viewport& viewport, Transform to_root);

 public:
    /**
     * Creates a new context from a parent context.
     *
     * The parent context must also derive from `BaseContext`. It must also
     * provide methods `inner_exporter` and `inner_viewport`, indicating the
     * viewport and exporter to use.
     */
    template <class ParentContext>
    explicit BaseContext(ParentContext& parent);
};

template <class Exporter>
BaseContext<Exporter>::BaseContext(const SvgDocument& document,
                                   spdlog::logger& logger, Exporter exporter,
                                   const Viewport& viewport, Transform to_root)
    : detail::BaseContextExporterless{document, logger, viewport, to_root},
      exporter_{exporter} {}

template <class Exporter>
template <class ParentContext>
BaseContext<Exporter>::BaseContext(ParentContext& parent)
    : detail::BaseContextExporterless{parent.document(), parent.logger(),
                                      parent.inner_viewport(),
                                      parent.to_root()},
      exporter_{parent.inner_exporter()} {}

#endif  // SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
