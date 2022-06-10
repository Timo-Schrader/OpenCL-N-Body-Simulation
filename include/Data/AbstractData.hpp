/**
 * @file AbstractData.hpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Base class for relevant Datasets
 * @version 1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __N_BODY_SIMULATION_ABSTRACTDATA_HPP__
#define __N_BODY_SIMULATION_ABSTRACTDATA_HPP_


#include "Float3.hpp"
#include <string>
#include <vector>

/**
 * @brief Base clase for all data sets
 * 
 */
class AbstractData {

protected:
    std::string name;    //!< Name of the data set
    std::size_t size = 0;//!< Number of bodies

    // ************ Change parameters if needed ************
    // These are the parameters for the random objects that
    // are created during initialization.
    // *****************************************************
    double pMax;//!< Maximum position in each direction (also used as w-coordinate in the shader)
    double pMin;//!< Minimum position in each direction
    double vMax;//!< Maximum velocity which can be obtained by each body
    double vMin;//!< Minimum velocity which can be obtained by each body
    double mMax;//!< Maximum mass of a body
    double mMin;//!< Minimum mass of a body
    // *****************************************************
    std::vector<float3> positions; //!< Contains float3 position structs for each body
    std::vector<float3> velocities;//!< Contains float3 velocity structs for each body
    std::vector<float> masses;     //!< Contains the mass of each body

    std::vector<float> flatPositions; //!< Since float scalars instead of float3 vectors are required in the rendering pipeline, this std::vector has three times the size of positions and holds all positions as continuous memory block which stride 3
    std::vector<float> flatVelocities;//!< Flat velocity values

public:
    std::string getName() const;//!< Returns the name of the data set
    std::size_t getSize() const;//!< Returns the amount of bodies used in the simulation
    std::size_t getBytesCount() const;

    std::vector<float3> getPositions(); //!< Returns positions array
    std::vector<float3> getVelocities();//!< Returns velocities array

    std::vector<float> getMasses() const;//!< Returns the mass array

    virtual std::vector<float> getFlatPositions() = 0; //!< Returns the flattened positions
    virtual std::vector<float> getFlatVelocities() = 0;//!< Returns the flattened velocities
    double getMaxPosition() const;                     //!< Returns the largest possible position value (required for the Vertex Shader)
    virtual double getMaxMass() const = 0;             //!< Returns the largest possible mass
    double getMinMass() const;                         //!< Returns the lowest possible mass

    friend double simulateCPU();
};


#endif
