@echo off

if exist benchmarks-build\ (
    rmdir /s /q benchmarks-build
)

mkdir benchmarks-build

REM Run GPU benchmarks with different kernels
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target ALL_BUILD --config Release
cd ..

benchmarks-build\Release\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device GPU --Benchmark LONG
benchmarks-build\Release\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody_local.cl --Device GPU --Benchmark LONG
benchmarks-build\Release\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody_async.cl --Device GPU --Benchmark LONG

REM Clean up
rmdir /s /q benchmarks-build
mkdir benchmarks-build

REM Run CPU benchmarks with OpenMP and SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=1 -DENABLE_SIMD=1 ..
cmake --build . --target ALL_BUILD --config Release
cd ..

benchmarks-build\Release\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device CPU --Benchmark SHORT

REM Clean up
rmdir /s /q benchmarks-build
mkdir benchmarks-build

REM Run CPU benchmarks without OpenMP but with SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=0 -DENABLE_SIMD=1 ..
cmake --build . --target ALL_BUILD --config Release
cd ..

benchmarks-build\Release\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device CPU --Benchmark SHORT

REM Clean up
rmdir /s /q benchmarks-build