/**
 * @file main.cpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Entry Point of the application and initialization of all devices
 * @version 1
 * @date 2021-12-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef __unix__
#include <GL/glx.h>
#elif _WIN32
#include <GL/GL.h>
#endif
// clang-format on
#include "../../include/Data/WikipediaDataSet.hpp"
#include "PerformanceMetrics/PerformanceMetricsCollector.hpp"
#include <boost/program_options.hpp>
#include <cassert>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <string>

#include <Render/render.hpp>
#include <Simulation/GPUCalc.hpp>

extern bool useGPU;
extern bool useCPU;
extern AbstractData *dataSet;
extern std::string kernelFile;
extern std::string kernelInputPath;

// for benchmark mode
extern std::vector<size_t> bodyNumbers;
extern BenchmarkMode benchmark;
std::string logFileName;
extern PerformanceMetricsCollector *performanceMetricsCollector;

/**
 * @brief Entry Point of the program
 * 
 * @param argc Command-line argument count
 * @param argv Command-line arguments
 * @return int Status Code
 */
int main(int argc, char *argv[]) {
    std::size_t N = 0;
    std::string dataSetName = "";
    kernelInputPath = "../kernels/";
    std::string bodyInitDistribution = "normal";
    std::string device = "GPU";
    //*************************
    // Add Command Line Options
    //*************************
    boost::program_options::options_description optionDescription("Command Line Options");
    optionDescription.add_options()("N", boost::program_options::value<int>(), "Define number of bodies (added to default amount of bodies)");
    optionDescription.add_options()("Max_Mass", boost::program_options::value<std::string>(), "Maximum Mass of random bodies");
    optionDescription.add_options()("Dataset", boost::program_options::value<std::string>(), "Name of dataset");
    optionDescription.add_options()("Random_Initialization", boost::program_options::value<std::string>(), "Random distribution used for initializing body positions; MUST BE 'uniform' or 'normal'");
    optionDescription.add_options()("CL_Kernel_Path", boost::program_options::value<std::string>(), "Path to OpenCL Kernel files");
    optionDescription.add_options()("Kernel", boost::program_options::value<std::string>(), "Kernel file to use");
    optionDescription.add_options()("Device", boost::program_options::value<std::string>(), "Device used for simulation; must be GPU, CPU or CPUGPU");
    optionDescription.add_options()("Benchmark", boost::program_options::value<std::string>(), "Run program in benchmark mode and save results; must be either SHORT or LONG");
    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionDescription), vm);
        boost::program_options::notify(vm);
    } catch (...) {
        std::cerr << "Please make sure that you only use well-formed command line arguments!\n";
        return 1;
    }


    if (vm.count("N")) {
        N = vm["N"].as<int>();
    }
    if (vm.count("Dataset")) {
        dataSetName = vm["Dataset"].as<std::string>();
    }
    if (vm.count("CL_Kernel_Path")) {
        kernelInputPath = vm["CL_Kernel_Path"].as<std::string>();
    }
    if (vm.count("Kernel")) {
        kernelFile = vm["Kernel"].as<std::string>();
    }
    if (vm.count("Random_Initialization")) {
        bodyInitDistribution = vm["Random_Initialization"].as<std::string>();
        if (bodyInitDistribution != "normal" && bodyInitDistribution != "uniform") {
            std::cerr << "Invalid initialization distribution given; must be 'uniform' or 'normal'\n";
            return 1;
        }
    }
    if (vm.count("Device")) {
        device = vm["Device"].as<std::string>();
        if (device != "CPU" && device != "GPU" && device != "CPUGPU") {
            std::cerr << "Device is invalid. Please specifiy 'CPU', 'GPU' or 'CPUGPU'.\n";
            return 2;
        }
    }
    benchmark = BenchmarkMode::OFF;
    if (vm.count("Benchmark")) {
        std::string mode = vm["Benchmark"].as<std::string>();
        benchmark = BenchmarkMode::SHORT;
        if (mode == "LONG") {
            benchmark = BenchmarkMode::LONG;
        }
    }

    if (device == "CPU") {
        useCPU = true;
    } else if (device == "GPU") {
        useGPU = true;
    } else {
        useCPU = true;
        useGPU = true;
    }

#ifdef _OPENMP
    std::cout << "Using at most " << omp_get_max_threads() << " threads for OpenMP.\n";
#endif

    //************************
    // Data Set Initialization
    //************************
    if (true) {
        if (vm.count("Max_Mass")) {
            const std::string sMaxMass = vm["Max_Mass"].as<std::string>();
            std::istringstream iss(sMaxMass);
            double maxMass;
            iss >> maxMass;
            dataSet = new WikipediaDataSet(N, bodyInitDistribution, maxMass);
        } else {
            dataSet = new WikipediaDataSet(N, bodyInitDistribution);
        }
    }

    if (benchmark == BenchmarkMode::OFF) {
        int resCode = openGlInit(argc, argv);
        openClInit();
        gpuInit();

        // initialize performance metrics
        performanceMetricsCollector = new PerformanceMetricsCollector();

        //Enter Main Render Loop
        glutMainLoop();

        // Freeing Memory
        delete dataSet;
    } else {
        //************************************************************************
        // Run multiple benchmark steps with different number of bodies
        //************************************************************************
        for (size_t i = 0; i < bodyNumbers.size(); ++i) {
            if (i < 6 || benchmark == BenchmarkMode::LONG) {
                // initialize data set
                dataSet = new WikipediaDataSet(bodyNumbers[i], "normal");
                std::cout << "Start benchmark iteration " << i << " with " << dataSet->getSize() << " bodies" << std::endl;

                // initialize everything
                openGlInit(argc, argv);
                openClInit();
                gpuInit();

                // In the first iteration we need to initialize the CSV file to save the results
                if (i == 0)
                    logFileName = PerformanceMetricsCollector::initLogFile();

                // start benchmark iteration
                glutMainLoop();
                performanceMetricsCollector->writeToLogFile(logFileName, dataSet->getSize());

                // Freeing Memory
                delete dataSet;
            } else {
                break;
            }
        }
    }

    return 0;
}
