#!/bin/bash

cd running_times/
# Translate latex files into PDF's
for f in ./*_plot.tex; do pdflatex $f; done
cd -

cd distributions/
./compile_figures.sh 
cd -
