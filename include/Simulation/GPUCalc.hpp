/**
* @file GPUcalc.hpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains declarations for GPU calculation functions
* @version 1
* @date 2022-01-04
*
* @copyright Copyright (c) 2022
*
*/
#ifndef __N_BODY_SIMULATION_GPUCALC_HPP__
#define __N_BODY_SIMULATION_GPUCALC_HPP__

#include "../../lib/OpenCL/Device.hpp"
#include <string>
#include <vector>

double simulateGPU();
void openClInit();
void gpuInit();
void compileKernel(const std::vector<cl::Device> &devices);


#endif
