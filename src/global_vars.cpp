/**
 * @file global_vars.cpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Contains global variables used throughout the program
 * @version 1
 * @date 2021-12-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// clang-format off
#include "../include/Data/AbstractData.hpp"
#include "../include/glm/mat4x4.hpp"
#include "PerformanceMetrics/PerformanceMetricsCollector.hpp"
#include <GL/glew.h>
#include <../lib/OpenCL/Device.hpp>
// clang-format on

bool useGPU = false;//!< Whether to use GPU for simulation
bool useCPU = false;//!< Whether to use CPU for simulation

int mainWindow;               //!< Handle of the main window which all content is being rendered to
GLuint vao;                   //!< Vertex array object which combines all vertex buffer objects
GLuint shaderProgram;         //!< Handle of the shader program
AbstractData *dataSet;        //!< Pointer for dataset object which can be specified during runtime
std::size_t numRenderElements;//!< Number of bodies to be rendered

int M_model_loc = 0;     //!< Location of the model matrix in the shader program
int M_view_loc = 0;      //!< Location of the view matrix in the shader program
int M_projection_loc = 0;//!< Location of the projection matrix in the shader program
int modeLoc = 0;         //!< Location of the uniform mode variable in the shader determining which kind of objects to draw
int zoomFactorLoc = 0;   //!< Location of the zoom factor in the shader

glm::mat4 M_model;     //!< Model matrix
glm::mat4 M_view;      //!< View matrix
glm::mat4 M_projection;//!< Projection matrix

glm::vec3 eyeVector(0.0f, 0.0f, 1.0f);                                         //!< Position of the camera
glm::vec3 centerVector(0.0f, 0.0f, 0.0f);                                      //!< Point to which the camera is looking
glm::vec3 yUpVector(0.0f, 1.0f, 0.0f);                                         //!< Vector in positive y-direction
glm::vec3 directionVector = glm::normalize(-(centerVector - eyeVector));       //!< Direction vector indicating camera direction
glm::vec3 rightVector = glm::normalize(glm::cross(yUpVector, directionVector));//!< Taking the cross product of two vectors yields a resulting vector which is perpendicular to both
glm::vec3 upVector = glm::normalize(glm::cross(directionVector, rightVector)); //!< Same logic as above to obtain perpendicular up vector of camera

float zoomFactor = 0.3f;             //<! Determines the zoom factor of the rendered screne; changed when scrolling
bool copyMatricesToGPU = false;      //!< If any of the matrices has been altered, they will all be copied again to the GPU in the render loop
bool automaticCameraRotation = false;//!< Whether to rotate the camera automatically at each timestep

//cl vars
cl::Kernel kernel;                  //!< kernel to calculate new values for all bodies
cl::Kernel updateKernel;            //!< kernel to update positions
cl::CommandQueue queue;             //!< queue to run commands on GPU
cl::Context context;                //!< the cl GPU context
std::string kernelFile = "nbody.cl";//!< kernel file to use
std::string kernelInputPath;        //!< path to kernel folder
cl::Buffer d_pos;                   //!< a buffer with flattened positions of all bodies
std::vector<float3> h_pos;         //!< Buffer for float3 positions on the host side
cl::Buffer d_vel;                  //!< a buffer with flattened velocities of all bodies
cl::Buffer d_masses;               //!< a buffer with masses of all bodies
std::vector<cl::Memory> mem_object;//!< mem object to share with OpenGL lib
int wgSize = 0;                    //!< size of the workgroup
inline const std::vector<float> coordinateSystemLines = {
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f};//!< Coordinates of the coord system axes

inline const std::vector<float> lineColors = {
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
};//!< Colors of the coord system axes

// Benchmark variables
std::vector<size_t> bodyNumbers{7, 119, 1015, 10231, 20471, 102391, 204791, 409591};
size_t benchmarkLength = 10;
size_t benchmarkWarmUp = 120;
std::string gpuName;
BenchmarkMode benchmark;
PerformanceMetricsCollector *performanceMetricsCollector;
