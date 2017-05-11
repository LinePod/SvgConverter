#!/bin/bash
docker run --rm -v $(realpath convert):/convert -v $(realpath container):/container:ro -v $(realpath build):/build linespace-svg-converter /bin/bash /container/run.sh
