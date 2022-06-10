#!/bin/bash

# This script automates the process of running
# the benchmarks with different kernels and devices.

mkdir -p benchmarks-build/

# Run GPU benchmarks with different kernels
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ..

benchmarks-build/N-Body-Simulation --CL_Kernel_Path kernels/ --Kernel nbody.cl --Device GPU --Benchmark LONG
benchmarks-build/N-Body-Simulation --CL_Kernel_Path kernels/ --Kernel nbody_local.cl --Device GPU --Benchmark LONG
benchmarks-build/N-Body-Simulation --CL_Kernel_Path kernels/ --Kernel nbody_async.cl --Device GPU --Benchmark LONG

# Clean up
rm -r benchmarks-build/
mkdir -p benchmarks-build/

# Run CPU benchmarks with OpenMP and SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=1 -DENABLE_SIMD=1 ..
make
cd ..

benchmarks-build/N-Body-Simulation --CL_Kernel_Path kernels/ --Kernel nbody.cl --Device CPU --Benchmark SHORT

# Clean up
rm -r benchmarks-build/
mkdir -p benchmarks-build/

# Run CPU benchmarks without OpenMP but with SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=0 -DENABLE_SIMD=1 ..
make
cd ..

benchmarks-build/N-Body-Simulation --CL_Kernel_Path kernels/ --Kernel nbody.cl --Device CPU --Benchmark SHORT

# Clean up
rm -r benchmarks-build/
