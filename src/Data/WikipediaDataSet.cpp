/**
 * @file WikipediaDataSet.cpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Contains implementations for Wikipedia Dataset class
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <algorithm>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <cmath>
#include <ctime>

#include "../include/Data/WikipediaDataSet.hpp"

/**
* @brief Construct a new Wikipedia Data Set object
* 
* @param numberRandomObjects Amount of bodies (+ 9 which are default)
* @param initDistribution Type of random distribution used for position initialization
* @param mMax Maximum possible mass
* @param mMin Minimum possible mass
* @param pMax Maximum possible position
* @param pMin Minimum possible position
* @param vMax Maximum possible velocity
* @param vMin Minimum possible velocity
*/
WikipediaDataSet::WikipediaDataSet(const std::size_t numberRandomObjects, std::string initDistribution, const double mMax, const double mMin, const double pMax, const double pMin, const double vMax, const double vMin) {
    this->name = "WIKIPEDIA DATA SET";
    this->pMax = pMax;
    this->pMin = pMin;
    this->vMax = vMax;
    this->vMin = vMin;
    this->mMax = mMax;
    this->mMin = mMin;

    // initialize object 0
    this->positions.emplace_back(0.0, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 0.0, 0.0);
    this->masses.emplace_back(1.989e30);
    this->size++;

    // initialize object 1
    this->positions.emplace_back(57.909e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 47.36e3, 0.0);
    this->masses.emplace_back(0.33011e24);
    this->size++;

    // initialize object 2
    this->positions.emplace_back(108.209e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 35.02e3, 0.0);
    this->masses.emplace_back(4.8675e24);
    this->size++;

    // initialize object 3
    this->positions.emplace_back(149.596e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 29.78e3, 0.0);
    this->masses.emplace_back(5.9724e24);
    this->size++;

    // initialize object 4
    this->positions.emplace_back(227.923e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 24.07e3, 0.0);
    this->masses.emplace_back(0.64171e24);
    this->size++;

    // initialize object 5
    this->positions.emplace_back(778.570e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 13e3, 0.0);
    this->masses.emplace_back(1898.19e24);
    this->size++;

    // initialize object 6
    this->positions.emplace_back(1433.529e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 9.68e3, 0.0);
    this->masses.emplace_back(568.34e24);
    this->size++;

    // initialize object 7
    this->positions.emplace_back(2872.463e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 6.80e3, 0.0);
    this->masses.emplace_back(86.813e24);
    this->size++;

    // initialize object 8
    this->positions.emplace_back(4495.060e9, 0.0, 0.0);
    this->velocities.emplace_back(0.0, 5.43e3, 0.0);
    this->masses.emplace_back(102.413e24);
    this->size++;

    // initialize random objects
    boost::random::mt19937 gen;
    gen.seed(std::time(0));
    boost::random::normal_distribution<> normalDistPos(0.0, 1e11);
    boost::random::uniform_real_distribution<> uniformDistPos(-pMax, pMax);
    boost::random::uniform_real_distribution<> distVel(vMin, vMax);
    // In order to obtain better random values, we use log10 to also consider numbers on the left side of the interval
    boost::random::uniform_real_distribution<> distMas(std::log10(mMin), std::log10(mMax));
    for (size_t i = 0; i < numberRandomObjects; ++i) {
        if (initDistribution == "normal") {
            this->positions.emplace_back(
                    normalDistPos(gen),
                    normalDistPos(gen),
                    normalDistPos(gen));
        } else if (initDistribution == "uniform") {
            this->positions.emplace_back(
                    uniformDistPos(gen),
                    uniformDistPos(gen),
                    uniformDistPos(gen));
        }
        this->velocities.emplace_back(
                distVel(gen),
                distVel(gen),
                distVel(gen));
        this->masses.emplace_back(std::pow(10, distMas(gen)));
        this->size++;
    }

    this->flatPositions.resize(3 * this->positions.size());
    this->flatVelocities.resize(3 * this->velocities.size());
}
std::vector<float> WikipediaDataSet::getFlatPositions() {
    for (std::size_t i = 0; i < this->positions.size(); ++i) {
        this->flatPositions.at(3 * i) = this->positions.at(i).x;
        this->flatPositions.at(3 * i + 1) = this->positions.at(i).y;
        this->flatPositions.at(3 * i + 2) = this->positions.at(i).z;
    };
    return this->flatPositions;
}

std::vector<float> WikipediaDataSet::getFlatVelocities() {
    for (std::size_t i = 0; i < this->velocities.size(); ++i) {
        this->flatVelocities.at(3 * i) = this->velocities.at(i).x;
        this->flatVelocities.at(3 * i + 1) = this->velocities.at(i).y;
        this->flatVelocities.at(3 * i + 2) = this->velocities.at(i).z;
    };
    return this->flatVelocities;
}

double WikipediaDataSet::getMaxMass() const {
    return 1.989e30 > this->mMax ? 1.989e30 : this->mMax;
}
