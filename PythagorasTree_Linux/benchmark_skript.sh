#!/bin/bash

set -e

for depth in 5 7 9 11 13 15 17 19 21;
do 
    for iteration in {1..7}
    do
        ./output/main --show false --serial-depth $depth --openmp-depth 0 --opencl-depth 0
    done
done

for depth in 5 7 9 11 13 15 17 19 21;
do 
    for iteration in {1..7}
    do
        ./output/main --show false --serial-depth 0 --openmp-depth $depth --opencl-depth 0
    done
done

for depth in 5 7 9 11 13 15 17 19 21; 
do 
    for iteration in {1..7}
    do
        ./output/main --show false --serial-depth 0 --openmp-depth 0 --opencl-depth $depth
    done
done