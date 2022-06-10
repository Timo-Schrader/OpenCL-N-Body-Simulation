/**
* @file PerformanceMetricsCollector.cpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Contains a class to collect and analyze the calculation times and frame rates of the GPU and CPU implementation
* @version 1
* @date 2022-01-17
*
* @copyright Copyright (c) 2022
*
*/

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../../include/PerformanceMetrics/PerformanceMetricsCollector.hpp"

extern bool useGPU;
extern bool useCPU;
extern std::string kernelFile;
extern std::string gpuName;

PerformanceMetricsCollector::PerformanceMetricsCollector() = default;

/**
 * @brief Function to get the exact CPU model (works only on Linux).
 *
 * @return std::string containing the CPU model if running on Linux
 */
std::string PerformanceMetricsCollector::getCPUModel() {
#ifdef __linux__
    std::string line;
    std::ifstream finfo("/proc/cpuinfo");
    while (getline(finfo, line)) {
        std::stringstream str(line);
        std::string itype;
        std::string info;
        if (getline(str, itype, ':') && getline(str, info) && itype.substr(0, 10) == "model name") {
            boost::algorithm::trim(info);
            return info;
        }
    }
#else
    return std::string("Unknown CPU");
#endif
}

/**
 * @brief Writes system information and a header to a CSV file were the benchmark results can be saved in.
 *
 * @return std::string path to a CSV file were the benchmark results can be saved
 */
std::string PerformanceMetricsCollector::initLogFile() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y_%m_%d-%H-%M-%S");
    auto time = oss.str();

    std::string description;
#ifdef __linux__
    description = "Linux (gcc)";
#elif _WIN32
    description = "Windows";
#else
    description = "Unknown OS";
#endif
    if (useGPU)
        description += " GPU (" + gpuName + ") Kernel: " + kernelFile;
    else
        description += " CPU (" + PerformanceMetricsCollector::getCPUModel() + ")";
#ifndef NDEBUG
    if (useCPU)
        description += " Debug Mode";
#else
    if (useCPU)
        description += " Release Mode";
#endif
#ifdef ENABLE_SIMD
    if (useCPU)
        description += " - with SIMD";
#endif

#ifdef _OPENMP
    if (useCPU)
        description += " - with OpenMP";
#endif


    std::string fileName = "benchmarks/" + time + ".csv";
    std::ofstream logFile;
    logFile.open(fileName);
    logFile << description << std::endl;
    logFile << "nbody,calc_min,calc_max,calc_avg,fps_min,fps_max,fps_avg" << std::endl;
    logFile.close();
    return fileName;
}

/**
 * @brief Saves performance metrics in each render cycle.
 *
 * This method is called in each frame to save the calculation time
 * and calculate the time difference to the last render cycle.
 * This information is later used to calculate the frame rate
 *
 * @param t time to calculate the new body positions in seconds
 */
void PerformanceMetricsCollector::addCalcTime(double t) {
    calcTimes.addTime(t);
    Core::TimeSpan currentTime = Core::getCurrentTime();
    Core::TimeSpan diff = currentTime - this->lastTime;
    renderTimes.addTime(diff.getSeconds());
    this->lastTime = currentTime;
}

/**
 * @brief This method is used to print the current average calculation time and frame rate (only used in non-benchmark mode).
 */
void PerformanceMetricsCollector::printResult() {
    std::cout << "Avg calc time: " << calcTimes.getAvgTime() << "s, ";
    std::cout << "Avg FPS: " << 1.0 / renderTimes.getAvgTime() << std::endl;
}

/**
 * @brief This method saves the benchmark result into the CSV file after a benchmark step.
 *
 * @param logFileName path to the CSV file were the benchmark results can be saved
 * @param nbody number of bodies used in the benchmark step
 */
void PerformanceMetricsCollector::writeToLogFile(const std::string &logFileName, const size_t nbody) const {
    std::ofstream logFile;
    logFile.open(logFileName, std::ios_base::app);
    logFile << nbody << ",";
    logFile << calcTimes.getMinTime() << "," << calcTimes.getMaxTime() << "," << calcTimes.getAvgTime() << ",";
    logFile << 1.0 / renderTimes.getMaxTime() << "," << 1.0 / renderTimes.getMinTime() << "," << 1.0 / renderTimes.getAvgTime() << std::endl;
    logFile.close();
}

PerformanceMetric PerformanceMetricsCollector::getCalcTimes() const {
    return this->calcTimes;
}
