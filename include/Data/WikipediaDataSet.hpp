/**
 * @file WikipediaDataSet.hpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Header File for specific Wikipedia Dataset
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __N_BODY_SIMULATION_WIKIPEDIADATASET_HPP__
#define __N_BODY_SIMULATION_WIKIPEDIADATASET_HPP__

#include "AbstractData.hpp"

/**
 * @brief Implements the Wikipedia Dataset as found here: https://en.wikipedia.org/wiki/N-body_simulation#Initialisation_of_Simulation_Parameters
 * 
 */
class WikipediaDataSet : public AbstractData {

public:
    WikipediaDataSet(const std::size_t numberRandomObjects = 0, std::string initDistribution = "normal", const double mMax = 1e20, const double mMin = 1e10, const double pMax = 5e12, const double pMin = 0, const double vMax = 5e3, const double vMin = 0);
    std::vector<float> getFlatPositions() override;
    std::vector<float> getFlatVelocities() override;
    double getMaxMass() const override;
};


#endif
