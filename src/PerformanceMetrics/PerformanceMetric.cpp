/**
* @file PerformanceMetric.h
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains a class to collect and analyze performance metrics for GPU and CPU
* @version 1
* @date 2022-01-07
*
* @copyright Copyright (c) 2022
*
*/
#include "../../include/PerformanceMetrics/PerformanceMetric.hpp"
#include <algorithm>
#include <numeric>

PerformanceMetric::PerformanceMetric() = default;

void PerformanceMetric::addTime(const double t) {
    this->times.push_back(t);
}

double PerformanceMetric::getMinTime() const {
    auto minElement =  std::min_element(this->times.begin(), this->times.end());
    return *minElement;
}

double PerformanceMetric::getMaxTime() const {
    auto maxElement = std::max_element(this->times.begin(), this->times.end());
    return *maxElement;
}

double PerformanceMetric::getAvgTime() const {
    auto const count = static_cast<float>(this->times.size());
    return std::reduce(this->times.begin(), this->times.end()) / count;
}
