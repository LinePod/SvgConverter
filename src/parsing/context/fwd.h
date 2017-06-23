#ifndef SVG_CONVERTER_PARSING_CONTEXT_FWD_H_
#define SVG_CONVERTER_PARSING_CONTEXT_FWD_H_

// Forward declarations for context classes. These are needed to define the
// child context factories in `factories.h`, which in turn are needed to define
// `DocumentTraversal` in `traversal.h`. Context implementations need
// `DocumentTraversal` to lookup referenced elements, which leads to a cyclic
// dependency without using forward declarations.

class BaseContext;

template <class Exporter>
class GraphicsElementContext;

template <class Exporter>
class GContext;

template <class Exporter>
class SvgContext;

template <class Exporter>
class ShapeContext;

template <class Exporter>
class PatternContext;

#endif  // SVG_CONVERTER_PARSING_CONTEXT_FWD_H_
