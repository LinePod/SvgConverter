# SVG simplifier

Converts SVGs into simple, line-based formats. Can currently convert SVGs into GPGL commands.

## Setup

Required dependencies are Boost, Eigen3 and LibXml2.
For building, CMake >= 3.7 and a modern C++14 compiler (GCC >= 5.0 or equivalent) is also required.

To build:

 * Clone repo including submodules: `git clone --recursive ...`
 * Create a build folder: `mkdir build`
 * Enter it: `cd build`
 * Run cmake to create makefiles (this is for a release build): `cmake -DCMAKE_BUILD_TYPE=Release ..`
 * Build the converter: `make svg_converter`
