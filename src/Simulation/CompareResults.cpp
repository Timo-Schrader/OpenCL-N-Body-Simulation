/**
* @file CompareResults.cpp
* @author Kay Scheerer, Fabian Hauck, Timo Schrader
* @brief Entry Point of the application and initialization of all devices
* @version 1
* @date 2022-01-15
*
* @copyright Copyright (c) 2022
*
*/

#include <iostream>

#include "../../include/Simulation/CompareResults.hpp"
#include "../../include/Data/AbstractData.hpp"

extern AbstractData *dataSet;
extern std::vector<float3> h_pos;

void compareResults() {
    // check if GPU and CPU results are the same
    float errorSum = 0.0;
    float avgValue = 0.0;
    for (size_t i=0; i<dataSet->getSize(); ++i) {
        errorSum += distance(h_pos[i], dataSet->getPositions()[i]);
        avgValue += std::sqrt(dot(dataSet->getPositions()[i], dataSet->getPositions()[i]));
    }
    avgValue /= (float) dataSet->getSize();
    errorSum /= (float) dataSet->getSize();
    std::cout << "Average value: "<< avgValue <<", Average error: " << errorSum << ", Relative error: " << errorSum / avgValue << std::endl;
}
