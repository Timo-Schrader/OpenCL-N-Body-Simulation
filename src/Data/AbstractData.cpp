/**
 * @file AbstractData.cpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Contains implementations for Abstract Dataset base class
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Data/AbstractData.hpp"

std::string AbstractData::getName() const {
    return this->name;
}

std::size_t AbstractData::getSize() const {
    return this->size;
}

std::vector<float> AbstractData::getMasses() const {
    return this->masses;
}

double AbstractData::getMaxPosition() const {
    return this->pMax;
}

double AbstractData::getMaxMass() const {
    return this->mMax;
}

double AbstractData::getMinMass() const {
    return this->mMin;
}

std::vector<float3> AbstractData::getPositions() {
    return this->positions;
}

std::vector<float3> AbstractData::getVelocities() {
    return this->velocities;
}

std::size_t AbstractData::getBytesCount() const {
    return this->size * 3 * sizeof(float);
}