# N-Body Simulation with OpenCL
Authors: Kay Scheerer, Fabian Hauck, Timo Schrader

![Build Status](https://github.com/KayARS/GPU_Prog_submission/actions/workflows/main.yml/badge.svg)

## Overview
This project implements a N-Body Simulation using OpenCL for hardware acceleration and OpenGL for rendering. Furthermore, it also provides a sequential reference implementation for the CPU which is also available as parallel variant through OpenMP and SIMD instructions.

Dataset and formulas have been taken from the following page: [Wikipedia N-Body Simulation](https://en.wikipedia.org/wiki/N-body_simulation#Example_Simulations)

![Nbody GIF](doc/nbody.gif)

## Build Instructions
**Please make sure that your hardware (GPU) and driver support OpenCL and OpenGL.**
### _Linux_
In order to build the project, please use the *configure* script to install all relevant dependencies. After that, execute the following:
```
cmake CMakeLists.txt
make
```
### _Windows_
It is also possible to build this program under Windows using the MSVC toolchain. You just need to install the relevant libraries and add their paths to the CMakeLists.txt. Please refer to this file and check which additional libraries you need to install. 

**Make sure to specify the CMake Build Type appropriately in your IDE/Editor! (i.e., "Release" or "Debug")**

**We also highly recommend using the Clang toolchain instead which can be installed with Visual Studio. There is a huge CPU performance drop using the Microsoft compiler. You'll need the Boost libraries compiled with Clang to do that (download the official Boost archive and run the build scripts from the VS 2022 developer command line to compile them).**
### **Additional Build Parameters**
It is furthermore possible to configure the build process by specifying one or more of the following CMake variables:
- _ENABLE_OPENMP_: Define this variable and set the string to a non-zero value to build the program with OpenMP enabled
- _ENABLE_SIMD_: Define this variable and set the string to a non-zero value to build the float3 class with SIMD vector instructions enabled
## Run the Program
The easiest way to run the program is to use the command line. The following command line parameters can/must be specified:
- \-\-N: The number of additional random bodies (defaults to 0)
- \-\-Max_Mass: Maximum mass of random bodies
- \-\-Dataset: The dataset that should be used (currently, only "Wikipedia" is available)
- \-\-Random_Initialization: The random distribution that is used to initialize random bodies; must be "normal" or "uniform"
- \-\-CL_Kernel_Path: Path to folder which contains OpenCL kernel source files; should be specified if the program can't find the kernel files on its own
- \-\-Kernel: Name of the compute kernel that should be used
- \-\-Device: Simulation calculation device; must be "CPU", "GPU" or "CPUGPU"
- \-\-Benchmark: If set, program will run in benchmark mode; must be SHORT or LONG

### Keyboard Shortcuts
The following keyboard shortcuts have been implemented for easier usage:
- 'R': Enable/Disable automatic camera rotation
- 'F': Enter/Leave fullscreen mode
- 'Esc': Close the program
## Project Structure
    .
    ├── .github                 # Contains Github Actions pipeline script
    ├── benchmarks              # Contains benchmark execution shell scripts
    ├── doc                     # Contains GIF files
    ├── doxygen_files           # Contains Doxyfile and the generated documentation
    ├── include                 # Contains header files and the GLM library
    ├── kernels                 # Contains OpenCL kernel source files
    ├── lib                     # Contains additional, external source files
    ├── shaders                 # Contains GLSL shader files
    ├── src                     # Contains source files
    ├── visualization           # Contains Python script to plot benchmark results
    ├── .clang-format
    ├── .gitignore
    ├── CMakeLists.txt
    ├── configure
    ├── LICENSE
    └── README.md

## Benchmark Program
In case you want to benchmark CPU and GPU, you can run the shell scripts provided in ./benchmarks. It can take several hours to finish. The results will be stored in CSV files which can then be visualized using the Python script in ./visualization. In case you are using Windows, please open a Visual Studio developer command prompt and run one of the batch scripts.
## Generate Documentation
In order to generate Code documentation, please install *doxygen* (see *configure*) and run the following:
```
cd doxygen_files
doxygen Doxyfile
```

## Used Libraries
This project uses the following additional libaries:
- [Boost](https://www.boost.org/)
- [FreeGLUT](http://freeglut.sourceforge.net/)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](https://github.com/g-truc/glm)