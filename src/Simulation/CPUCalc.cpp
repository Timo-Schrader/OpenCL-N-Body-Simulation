/**
* @file CPUCalc.cpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains definitions for CPU calculation functions
* @version 1
* @date 2022-01-04
*
* @copyright Copyright (c) 2022
*
*/

#include "../../include/Simulation/CPUCalc.hpp"
#include "../../include/Data/AbstractData.hpp"
#include "../../include/PerformanceMetrics/PerformanceMetric.hpp"
// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>

#ifdef __unix__
#include <GL/glx.h>
#elif _WIN32
#include <GL/GL.h>
#endif

// clang-format on
#include "../../lib/Core/Time.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include <iostream>

extern GLuint vbo;

extern AbstractData *dataSet;

#define p dataSet->positions
#define v dataSet->velocities
#define m dataSet->masses

float dt = 86400;
float BIG_G = 6.67e-11;

double simulateCPU() {
    Core::TimeSpan timeCPU1 = Core::getCurrentTime();

#pragma omp parallel for default(none) shared(dataSet, dt, BIG_G)
    for (std::size_t i = 0; i < dataSet->getSize(); ++i) {
        float3 acceleration(0.0);
        for (size_t j = 0; j < dataSet->getSize(); ++j) {
            if (i != j) {
                float3 r_vector = p[i] - p[j];
                float r_mag = std::sqrt(dot(r_vector, r_vector));
                float acc = -1.0f * BIG_G * (m[j] / std::pow(r_mag, 2.0));
                float3 r_unit_vector = r_vector / r_mag;
                acceleration += r_unit_vector * acc;
            }
        }
        v[i] += acceleration * dt;
    }


#pragma omp parallel for default(none) shared(dataSet, dt)
    for (std::size_t i = 0; i < dataSet->getSize(); ++i) {
        p[i] += v[i] * dt;
    }

    Core::TimeSpan timeCPU2 = Core::getCurrentTime();
    Core::TimeSpan executionTime = timeCPU2 - timeCPU1;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, dataSet->getFlatPositions().data(), dataSet->getBytesCount());
    glUnmapBuffer(GL_ARRAY_BUFFER);

    return executionTime.getSeconds();
}