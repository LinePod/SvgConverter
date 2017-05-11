This directory contains a docker container to easily run the converter on all systems.

To build the container and converter, run `./build.sh`.

To convert, place an SVG file in the `convert` directory and run `./run.sh`.
This will result in a `.gpgl` and `.err` file with the same basename to be generated in the `convert` directory.
