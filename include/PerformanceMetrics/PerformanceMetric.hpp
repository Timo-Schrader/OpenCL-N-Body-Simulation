/**
* @file PerformanceMetric.hpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains a class to collect and analyze performance metrics for GPU and CPU
* @version 1
* @date 2022-01-07
*
* @copyright Copyright (c) 2022
*
*/

#ifndef N_BODY_SIMULATION_PERFORMANCEMETRICS_H
#define N_BODY_SIMULATION_PERFORMANCEMETRICS_H

#include <limits>
#include <string>
#include <vector>

class PerformanceMetric {

private:
    std::vector<double> times;

public:
    PerformanceMetric();
    void addTime(double t);   // t in seconds
    double getMinTime() const;// in seconds
    double getMaxTime() const;// in seconds
    double getAvgTime() const;// in seconds
};

#endif//N_BODY_SIMULATION_PERFORMANCEMETRICS_H
