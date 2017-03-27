#ifndef SVG_CONVERTER_FWD_H
#define SVG_CONVERTER_FWD_H

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

#endif  // SVG_CONVERTER_FWD_H
