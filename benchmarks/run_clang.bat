@echo off

echo "You might want to run the following before executing this script: <C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x64>"
echo "Please adapt the paths to Clang s.t. it fits your setup!"

if exist benchmarks-build\ (
    rmdir /s /q benchmarks-build
)

mkdir benchmarks-build

REM Run GPU benchmarks with different kernels
cd benchmarks-build || exit 1
cmake -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" -G Ninja
cmake --build . --target all --config Release
cd ..

benchmarks-build\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device GPU --Benchmark LONG
benchmarks-build\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody_local.cl --Device GPU --Benchmark LONG
benchmarks-build\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody_async.cl --Device GPU --Benchmark LONG

REM Clean up
rmdir /s /q benchmarks-build

mkdir benchmarks-build

REM Run CPU benchmarks with OpenMP and SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=1 -DENABLE_SIMD=1 .. -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" -G Ninja
cmake --build . --target all --config Release
cd ..

benchmarks-build\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device CPU --Benchmark SHORT

REM Clean up
rmdir /s /q benchmarks-build
mkdir benchmarks-build

REM Run CPU benchmarks without OpenMP but with SIMD
cd benchmarks-build || exit 1
cmake -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=0 -DENABLE_SIMD=1 .. -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang.exe" -G Ninja
cmake --build . --target all --config Release
cd ..

benchmarks-build\N-Body-Simulation.exe --CL_Kernel_Path kernels\ --Kernel nbody.cl --Device CPU --Benchmark SHORT

REM Clean up
rmdir /s /q benchmarks-build
