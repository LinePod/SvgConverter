#ifndef SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_BASE_H_

#include <spdlog/spdlog.h>

#include "../../svg.h"

/**
 * Base context class.
 *
 * Contains some global data and some useful default implementations for SVG++
 * event handlers.
 */
class BaseContext {
 protected:
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

    explicit BaseContext(const SvgDocument& document, spdlog::logger& logger);

 public:
    /**
     * SVG++ event called when leaving an element.
     *
     * Needs to be defined for every context but seldom contains any logic,
     * therefore defined here to avoid boilerplate.
     */
    void on_exit_element();

    /**
     * Whether to process child elements of this element.
     *
     * Defaults to always return true. Reported to SVG++ via
     * `DocumentTraversalControlPolicy`.
     */
    bool process_children() const;
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
