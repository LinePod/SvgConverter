#!/bin/bash
docker build -t linespace-svg-converter .
docker run --rm -v $(realpath ..):/src:ro -v $(realpath container):/container:ro -v $(realpath build):/build linespace-svg-converter /bin/bash /container/build.sh
