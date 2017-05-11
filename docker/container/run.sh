#!/bin/bash
for IN_FILE in /convert/*.svg; do
    OUT_FILE="${IN_FILE%.svg}.gpgl"
    ERR_FILE="${IN_FILE%.svg}.err"
    /build/svg_converter $IN_FILE > $OUT_FILE 2> $ERR_FILE
done
