#!/bin/bash

mkdir -p build
cd build
cmake ../../../
make gen
cd ../
rm -f data.txt
./build/gen -b 11 -e 11 -d uint32 > data.txt
sqlplot-tools dist_plot.tex
