#!/bin/sh
cpplint --recursive --quiet --verbose=0 --filter=-legal,-build/header_guard,-build/include_subdir src
