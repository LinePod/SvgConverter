# SVG converter

Converts SVG images into GPGL commands.

## Supported features

The converter does not support the entire huge SVG specification, but only the most commonly used features.

Supported are:
  * All shapes with all attributes adjusting the form of them
    * `<rect>` (`x`, `y`, `width`, `height`, `rx`, `ry`)
    * `<line>` (`x1`, `y1`, `x2`, `y2`)
    * `<circle>` (`cx`, `cy`, `r`)
    * `<ellipse>` (`cx`, `cy`, `rx`, `ry`)
    * `<polygon>` and `<polyline>` (`points`)
    * `<path>` (`d`)
  * Tracing the outline of shapes
    * Active by default, can be disabled by setting `stroke="none"`
    * Dashed/dotted lines via `stroke-dasharray`
  * Filling shapes with patterns defined in the same SVG
    * Patterns can use patterns themselves
  * Layout features
    * Grouping shapes with `<g>` elements
      * `<g>` elements can currently only be used for layout, not for setting other attributes for multiple shapes
    * `transform` attribute
    * All layout attributes on `<svg>` elements (`x`, `y`, `width`, `height`, `viewBox`, `preserveAspectRatio`)
    * All layout attributes on `<pattern>` elements (`x`, `y`, `width`, `height`, `viewBox`, `preserveAspectRatio`, `patternUnits`, `patternContentUnits`)
    * Nesting `<svg>` elements

## Setup

Required dependencies are Boost, Eigen3, LibXml2, spdlog, SVG++ and Clipper.
The latter two are included in the repository.
For compilation, CMake >= 3.7 and a modern C++14 compiler (GCC >= 5.0 or equivalent) is required.

To build:

 * Clone repo including submodules: `git clone --recursive ...`
 * Create a build folder: `mkdir build`
 * Enter it: `cd build`
 * Run cmake to create makefiles (this is for a release build): `cmake -DCMAKE_BUILD_TYPE=Release ..`
 * Build the converter: `make svg_converter`

## Docker container
Compiling the converter is straightforward on Linux and macOS, but may be a bit more involved on Windows.
To make quick testing or conversion of SVG images easier, this repository also contains a Docker container.
For more details see the Readme in the `docker` directory.
