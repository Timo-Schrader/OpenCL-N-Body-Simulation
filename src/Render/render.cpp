/**
 * @file render.cpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Contains the definition for the render callback function
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */
typedef unsigned int uint;

#include "../../include/Render/render.hpp"
// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef __unix__
#include <GL/glx.h>
#elif _WIN32
#include <GL/GL.h>
#endif
// clang-format on
#include "../../include/glm/gtc/type_ptr.hpp"
#include "../../include/glm/mat4x4.hpp"
#include "./Simulation/CPUCalc.hpp"
#include "./Simulation/GPUCalc.hpp"

#include <iostream>

//cl imports
#include <../../lib/OpenCL/Device.hpp>
#include <../../lib/OpenCL/Event.hpp>
#include <../../lib/OpenCL/Program.hpp>
#include <../../lib/OpenCL/cl-patched.hpp>

#include "../../include/Render/render.hpp"
#include "../../include/Simulation/CompareResults.hpp"
#include "../../include/callbacks.hpp"
#include "../../include/constants.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"
#include "../../include/glm/mat4x4.hpp"
#include "../../lib/Core/Time.hpp"
#include "../../include/PerformanceMetrics/PerformanceMetric.hpp"

#include "../../include/Data/WikipediaDataSet.hpp"
#include "../../include/PerformanceMetrics/PerformanceMetricsCollector.hpp"

bool initialRun = true;//!< Determines whether the render loop is being executed for the first time

extern bool useGPU;
extern bool useCPU;

GLuint vbo;
GLuint mbo;
GLuint lbo;
GLuint lco;
extern GLuint vao;
extern GLuint shaderProgram;
extern int mainWindow;
extern AbstractData *dataSet;
extern std::size_t numRenderElements;


extern glm::vec3 eyeVector;
extern glm::vec3 centerVector;
extern glm::vec3 upVector;


extern int M_model_loc;
extern int M_view_loc;
extern int M_projection_loc;
extern int modeLoc;
extern int zoomFactorLoc;

extern glm::mat4 M_model;
extern glm::mat4 M_view;
extern glm::mat4 M_projection;

extern float zoomFactor;
extern bool copyMatricesToGPU;
extern bool automaticCameraRotation;

extern const std::vector<float> coordinateSystemLines;
extern const std::vector<float> lineColors;

// benchmark
double executionTime;
size_t nFrames = 0;
extern size_t benchmarkLength;
extern size_t benchmarkWarmUp;
extern BenchmarkMode benchmark;
extern PerformanceMetricsCollector *performanceMetricsCollector;


/**
 * @brief 
 * 
 * @param cpu Whether to let CPU calculate
 * @param gpu Whether to let GPU calculate
 * 
 */
void calcSimulationStep(bool cpu, bool gpu) {
    if (gpu) {
        executionTime = simulateGPU();
    }
    if (cpu) {
        executionTime = simulateCPU();
    }
    if (gpu && cpu) {
        compareResults();
    }

    performanceMetricsCollector->addCalcTime(executionTime);
    nFrames++;
    if (benchmark == BenchmarkMode::OFF) {
        performanceMetricsCollector->printResult();
    } else if (nFrames == benchmarkLength) {
        if (performanceMetricsCollector->getCalcTimes().getAvgTime() < 1.0) {
            std::cout << "Avg calc time is below 1 second => using warm up" << std::endl;
        } else {
            initialRun = true;
            nFrames = 0;
            glutLeaveMainLoop();
        }
    } else if (nFrames == benchmarkWarmUp) {
        delete performanceMetricsCollector;
        performanceMetricsCollector = new PerformanceMetricsCollector();
    } else if (nFrames == (benchmarkWarmUp + benchmarkLength)) {
        initialRun = true;
        nFrames = 0;
        glutLeaveMainLoop();
    }
}

extern bool exitRenderLoop;

/**
 * @brief Rendering function which is called repeatedly by glutMainLoop(); its task is to copy altered matrices to the GPU, draw all bodies that are pending
 * in the buffer and manage all necessary recalculations for the simulation
 */
void render() {
    if (initialRun) {
        modeLoc = glGetUniformLocation(shaderProgram, "mode");
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
        performanceMetricsCollector = new PerformanceMetricsCollector;
        initialRun = false;
    }
    if (copyMatricesToGPU) {
        glUniformMatrix4fv(M_model_loc, 1, GL_FALSE, glm::value_ptr(M_model));
        glUniformMatrix4fv(M_view_loc, 1, GL_FALSE, glm::value_ptr(M_view));
        glUniformMatrix4fv(M_projection_loc, 1, GL_FALSE, glm::value_ptr(M_projection));
        glUniform1f(zoomFactorLoc, zoomFactor);
        copyMatricesToGPU = false;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    // Drawing Bodies
    glUniform1ui(modeLoc, 0);
    glDrawArrays(GL_POINTS, 0, numRenderElements);
    // Drawing Coordinate System Axes
    glUniform1ui(modeLoc, 1);
    glDrawArrays(GL_LINES, 0, 6);
    glutSwapBuffers();
    calcSimulationStep(useCPU, useGPU);
    if (automaticCameraRotation) {
        M_view = glm::rotate(M_view, 0.01f, glm::vec3(0, 1, 0));
        copyMatricesToGPU = true;
    }
}


int openGlInit(int argc, char *argv[]) {
    //**********************
    // OpenGL initialization
    //**********************
    glutInit(&argc, argv);
    glutInitWindowPosition(-1, -1);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    mainWindow = glutCreateWindow(WINDOW_TITLE);
    glutDisplayFunc(render);
    glutKeyboardFunc(glKeyboardCallback);
    glutMotionFunc(glMouseMovementClickCallback);
    glutCloseFunc(glutCleanup);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#ifdef __unix__
    glutMouseFunc(glMouseWheelCallback);
#elif _WIN32
    glutMouseWheelFunc(glMouseWheelCallback);
#endif
    glutIdleFunc(glutPostRedisplay);

    glewInit();

    // Initialize Shaders
    // clang-format off
    std::string vertexShaderCode =
    #include VERTEX_SHADER_PATH
    ;
    std::string fragmentShaderCode =
    #include FRAGMENT_SHADER_PATH
    ;
    // clang-format on
    GLint isCompiled = -1;
    char shaderInfoLog[512];

    const GLchar *shaderCode = vertexShaderCode.c_str();
    assert(nullptr != shaderCode);
    uint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shaderCode, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (GL_FALSE == isCompiled) {
        glGetShaderInfoLog(vertexShader, 512, NULL, shaderInfoLog);
        std::cerr << "Vertex Shader compilation failed. Error: " << shaderInfoLog << "\n";
        glDeleteShader(vertexShader);
        return isCompiled;
    }

    shaderCode = fragmentShaderCode.c_str();
    assert(nullptr != shaderCode);
    uint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shaderCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (GL_FALSE == isCompiled) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, shaderInfoLog);
        std::cerr << "Fragment Shader compilation failed. Error: " << shaderInfoLog << "\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return isCompiled;
    }


    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate Vertex Buffer
    vbo = 0;
    glGenBuffers(1, &vbo);

    // Generate Color Buffer
    mbo = 0;
    glGenBuffers(1, &mbo);

    // Generate Line Buffer
    lbo = 0;
    glGenBuffers(1, &lbo);

    // Generate Line Color Buffer
    lco = 0;
    glGenBuffers(1, &lco);

    vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataSet->getFlatPositions().size(), dataSet->getFlatPositions().data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexArrayAttrib(vao, 0);


    glBindBuffer(GL_ARRAY_BUFFER, mbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataSet->getMasses().size(), dataSet->getMasses().data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexArrayAttrib(vao, 1);

    glBindBuffer(GL_ARRAY_BUFFER, lbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coordinateSystemLines.size(), coordinateSystemLines.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexArrayAttrib(vao, 2);

    glBindBuffer(GL_ARRAY_BUFFER, lco);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lineColors.size(), lineColors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexArrayAttrib(vao, 3);

    glLineWidth(COORD_AXIS_WIDTH);

    GLenum errCode = glGetError();
    if (GL_NO_ERROR != errCode) {
        std::cerr << "OpenGL encountered the following error: " << errCode << "\n";
        return errCode;
    }

    // Init Vertex Matrices
    M_model = glm::mat4(1.0f);
    M_view = glm::lookAt(eyeVector, centerVector, upVector);
    M_projection = glm::perspective(glm::radians(FOV), static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.001f, 300.0f);

    M_model_loc = glGetUniformLocation(shaderProgram, "M_model");
    M_view_loc = glGetUniformLocation(shaderProgram, "M_view");
    M_projection_loc = glGetUniformLocation(shaderProgram, "M_projection");
    glUniformMatrix4fv(M_model_loc, 1, GL_FALSE, glm::value_ptr(M_model));
    glUniformMatrix4fv(M_view_loc, 1, GL_FALSE, glm::value_ptr(M_view));
    glUniformMatrix4fv(M_projection_loc, 1, GL_FALSE, glm::value_ptr(M_projection));

    int minMass_loc = glGetUniformLocation(shaderProgram, "minMass");
    int maxMass_loc = glGetUniformLocation(shaderProgram, "maxMass");
    glUniform1f(minMass_loc, dataSet->getMinMass());
    glUniform1f(maxMass_loc, dataSet->getMaxMass());

    int maxPos_loc = glGetUniformLocation(shaderProgram, "maxPos");
    glUniform1f(maxPos_loc, dataSet->getMaxPosition());
    numRenderElements = dataSet->getSize();

    zoomFactorLoc = glGetUniformLocation(shaderProgram, "zoomFactor");
    glUniform1f(zoomFactorLoc, zoomFactor);
    return 0;
}

void glutCleanup() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &mbo);
    glDeleteBuffers(1, &lbo);
    glDeleteBuffers(1, &lco);
}