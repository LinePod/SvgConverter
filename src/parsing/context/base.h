#ifndef SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
#define SVG_CONVERTER_PARSING_CONTEXT_BASE_H_

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

    explicit BaseContext(const SvgDocument& document) : document_{document} {}

 public:
    /**
     * SVG++ event called when leaving an element.
     *
     * Needs to be defined for every context but seldom contains any logic,
     * therefore defined here to avoid boilerplate.
     */
    void on_exit_element() {}
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_BASE_H_
