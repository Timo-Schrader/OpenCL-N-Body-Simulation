/**
 * @file GPUCalc.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#define CL_TARGET_OPENCL_VERSION 300
#define CL ENABLE_EXCEPTIONS

// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>

#ifdef __unix__
#include <GL/glx.h>
#elif _WIN32
#include <GL/GL.h>
#endif

// clang-format on
#include "../../include/Simulation/GPUCalc.hpp"
#include "../../include/Data/AbstractData.hpp"
//open cl
#include "../../include/constants.hpp"
#include "../../lib/OpenCL/Device.hpp"
#include "../../lib/OpenCL/Error.hpp"
#include "../../lib/OpenCL/Event.hpp"
#include "../../lib/OpenCL/Program.hpp"
#include "../../lib/OpenCL/cl-patched.hpp"
//end
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include <iostream>

extern GLuint vbo;

extern bool useGPU;
extern bool useCPU;
//cl vars
extern AbstractData *dataSet;
//cl externs
extern cl::Kernel kernel;
extern cl::Kernel updateKernel;
extern cl::CommandQueue queue;
extern cl::Context context;
extern cl::Buffer d_pos;
extern cl::Buffer d_vel;
extern cl::Buffer d_masses;

extern int wgSize;
// cl::Event event;
extern std::vector<cl::Memory> mem_object;
extern int wgSize;
extern std::string kernelFile;
extern std::string kernelInputPath;
cl::Device device;
std::vector<std::size_t> maxWorkItems;
std::vector<std::size_t> maxWorkG;
cl::NDRange workGroupRange;
cl::NDRange overallItemRange;

extern std::vector<float3> h_pos;

#ifdef ENABLE_SIMD
float *tmpBuffer;
#endif

/**
 * @brief compiles the chosen kernels
 * 
 * @param devices devices to run kernel on
 */
void compileKernel(const std::vector<cl::Device> &devices) {
    cl::Program program = OpenCL::loadProgramSource(context, kernelInputPath + kernelFile);
    OpenCL::buildProgram(program, devices);
    kernel = cl::Kernel(program, "nbody_force_calculation");

    cl::Program updateProg = OpenCL::loadProgramSource(context, kernelInputPath + "updateKernel.cl");
    OpenCL::buildProgram(updateProg, devices);
    updateKernel = cl::Kernel(updateProg, "updateKernel");
}

/**
 * @brief inits open cl parameters, context, device, queue
 * 
 */
void openClInit() {
    //**********************
    // OpenCL initialization
    //**********************
    cl_uint num_platforms;
    clGetPlatformIDs(0, NULL, &num_platforms);
    cl_platform_id *platforms = new cl_platform_id[sizeof(cl_platform_id) * num_platforms];
    clGetPlatformIDs(num_platforms, platforms, NULL);
#ifdef __unix__
    cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
            CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0],
            0};
#elif _WIN32
    cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0],
            0};
#endif
    context = cl::Context(CL_DEVICE_TYPE_GPU, properties);

    int deviceNr = DEFAULT_OPENCL_DEVICE;
    std::cout << "Using device " << deviceNr << " / " << context.getInfo<CL_CONTEXT_DEVICES>().size() << std::endl;
    assert(deviceNr > 0);
    assert((size_t) deviceNr <= context.getInfo<CL_CONTEXT_DEVICES>().size());
    device = context.getInfo<CL_CONTEXT_DEVICES>()[deviceNr - 1];


    std::vector<cl::Device> devices;
    devices.push_back(device);
    OpenCL::printDeviceInfo(std::cout, device);
    compileKernel(devices);

    //TODO: dynamic local memory usage depending on device size

    // Create a command queue
    queue = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE);

    if (useCPU && useGPU) {
        d_pos = cl::Buffer(context, CL_MEM_READ_WRITE, dataSet->getBytesCount());
        h_pos.resize(dataSet->getSize());
    } else {
        d_pos = cl::BufferGL(context, CL_MEM_READ_WRITE, vbo);
    }
    mem_object.clear();
    mem_object.push_back(d_pos);
#ifdef ENABLE_SIMD
    tmpBuffer = new float[3 * dataSet->getSize() * sizeof(float)];
#endif
    delete[] platforms;
}

/**
 * @brief multiplies mass with G once 
 * 
 * @return std::vector<float> of masses
 */
std::vector<float> massInit() {
    std::vector<float> preCalcMasses;
    float G = -6.67e-11;
    for (std::size_t i = 0; i < dataSet->getSize(); i++) {
        preCalcMasses.emplace_back(dataSet->getMasses().at(i) * G);
    }
    return preCalcMasses;
}


/**
 * @brief inits everything on the gpu which has to be done only once and not at each render step
 * Inits arguments, buffers and NDRanges
 * 
 */
void gpuInit() {
    /*
    *First we write the important buffers, position, velocioty and masses to GPU buffers
    */
    int floatsize = sizeof(float);
    // masses, acc, pos and vel are needed, where as the later two will be copied from GL directly and don't need init here
    d_masses = cl::Buffer(context, CL_MEM_READ_ONLY, dataSet->getSize() * floatsize);
    queue.enqueueWriteBuffer(d_masses, true, 0, dataSet->getSize() * floatsize, massInit().data());

    int flatSize = dataSet->getFlatVelocities().size() * floatsize;
    d_vel = cl::Buffer(context, CL_MEM_READ_WRITE, flatSize);
    queue.enqueueWriteBuffer(d_vel, true, 0, flatSize, dataSet->getFlatVelocities().data());

    // gets the maximum work items allowed in a group for the device given
    maxWorkItems = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
    int workitems = maxWorkItems[0] / 16;

    /*
    *Second we set arguments for the different kernels
    */

    // arguments which are the same for every calculation kernel
    cl_int nrBodies = dataSet->getSize();
    kernel.setArg<cl::Buffer>(0, d_pos);
    kernel.setArg<cl::Buffer>(1, d_vel);
    kernel.setArg<cl::Buffer>(2, d_masses);
    kernel.setArg(3, nrBodies);


    // If you read the /16 you might be thinking: why would they only use a quarter of the local memory?
    // well the reason is: it runs faster.
    int loc_mem = device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() / 4;
    float dimSize = ((float) dataSet->getSize()) / workitems;
    int roundedDimSize = std::ceil(dimSize);
    overallItemRange = cl::NDRange(workitems * roundedDimSize);
    workGroupRange = cl::NDRange(workitems);


    /*
    * arguments for update kernel (all kernels use the same)
    */
    updateKernel.setArg<cl::Buffer>(0, d_pos);
    updateKernel.setArg<cl::Buffer>(1, d_vel);
    updateKernel.setArg(2, nrBodies);


    // if not nbody.cl:
    // both kernels have the same logic,
    // one just uses async copy, the other is self implemented copy (quite complex)
    // (they wouldnt acctually need the same parameters, but for simplicity, they do the same, they get the same)
    if (kernelFile == "nbody_local.cl" || kernelFile == "nbody_async.cl") {
        int floatsFitting = loc_mem / sizeof(float) - 4;


        //bodies per cycle (16 Bytes needed per Body,
        // workitems=Max amount of work items at any time)
        cl_int bodiesPerCycle = std::ceil(loc_mem / (16 * workitems));
        kernel.setArg(4, bodiesPerCycle);
        // 16 Bytes needed per body (4 floats à 4 Bytes)
        // but 4 Bytes are reserved (for something)
        cl_int maxNrBodiesInLocalMem = floatsFitting / 4;
        kernel.setArg(5, maxNrBodiesInLocalMem);

        // local position alloc
        kernel.setArg(6, cl::Local(3 * floatsFitting));
        // local masses alloc
        kernel.setArg(7, cl::Local(floatsFitting));
    }
    queue.finish();
}

/**
 * @brief gpu rendering methods with OpenGL bridge
 * 
 * @returns time need for calculation in seconds
 */
double simulateGPU() {
    if (useCPU && useGPU) {
        queue.enqueueWriteBuffer(d_pos, true, 0, dataSet->getBytesCount(), dataSet->getFlatPositions().data());
        queue.enqueueWriteBuffer(d_vel, true, 0, dataSet->getBytesCount(), dataSet->getFlatVelocities().data());
    } else {
        glFinish();
        cl_int err = queue.enqueueAcquireGLObjects(&mem_object);
        queue.finish();
    }

    // launches the kernel to calculate velocities
    cl::Event event;
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, overallItemRange, workGroupRange, nullptr, &event);
    queue.finish();

    // launces kernel to update positions based on velocities
    cl::Event updateEvent;
    queue.enqueueNDRangeKernel(updateKernel, cl::NullRange, overallItemRange, workGroupRange, nullptr, &updateEvent);
    queue.finish();

    Core::TimeSpan calcTime = OpenCL::getElapsedTime(event);
    Core::TimeSpan updateTime = OpenCL::getElapsedTime(updateEvent);

    if (useCPU && useGPU) {
#ifdef ENABLE_SIMD
        queue.enqueueReadBuffer(d_pos, true, 0, dataSet->getBytesCount(), tmpBuffer);
        for (std::size_t i = 0; i < dataSet->getSize(); ++i) {
            h_pos[i].x = tmpBuffer[3 * i];
            h_pos[i].y = tmpBuffer[3 * i + 1];
            h_pos[i].z = tmpBuffer[3 * i + 2];
            h_pos[i].w = EPSILON;
        }
#else
        queue.enqueueReadBuffer(d_pos, true, 0, dataSet->getBytesCount(), h_pos.data());
#endif

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, h_pos.data(), dataSet->getBytesCount());
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        queue.enqueueReleaseGLObjects(&mem_object);
        queue.finish();
    }
    return calcTime.getSeconds() + updateTime.getSeconds();
}