#!/bin/bash

cd distributions/
./generate.sh
cd -

cd running_times/
# Decompress measurements
7z e results.7z
# Load result files into data base
sqlplot-tools load_database/load_database.tex
# Create some additional tables containing aggregated data
# For speed and clean code
sqlplot-tools preprocess_data.tex
cd -

cd  running_times/R/
# Generate data for performance plots
for f in ./*R; do Rscript $f; done
cd -

cd running_times/
# Evaluate code in latex files to insert data points
for f in ./*.tex; do sqlplot-tools $f; done
cd -
