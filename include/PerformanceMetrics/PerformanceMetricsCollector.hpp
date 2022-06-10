/**
* @file PerformanceMetricsCollector.hpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains a class to collect and analyze the calculation times and frame rates of the GPU and CPU implementation
* @version 1
* @date 2022-01-17
*
* @copyright Copyright (c) 2022
*
*/

#ifndef N_BODY_SIMULATION_PERFORMANCEMETRICSCOLLECTOR_H
#define N_BODY_SIMULATION_PERFORMANCEMETRICSCOLLECTOR_H

#include "../../lib/Core/Time.hpp"
#include "../../lib/Core/TimeSpan.hpp"
#include "PerformanceMetric.hpp"

enum class BenchmarkMode { OFF,
                           SHORT,
                           LONG };

class PerformanceMetricsCollector {
private:
    PerformanceMetric calcTimes;
    PerformanceMetric renderTimes;
    Core::TimeSpan lastTime = Core::getCurrentTime();
    static std::string getCPUModel();

public:
    PerformanceMetricsCollector();
    void addCalcTime(double t);
    void printResult();
    void writeToLogFile(const std::string &logFileName, size_t nbody) const;
    static std::string initLogFile();
    PerformanceMetric getCalcTimes() const;
};


#endif//N_BODY_SIMULATION_PERFORMANCEMETRICSCOLLECTOR_H
