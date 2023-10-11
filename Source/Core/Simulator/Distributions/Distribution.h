//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Distribution class, representing
                 the base class for all continuous probability distributions.
    Additional Notes: None
    Date Created: 2023-10-10
*/

#pragma once

#include <vector>
#include <tuple>
#include <random>
#include <cstdint>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

/**
 * @brief This struct provides the base class for all continuous probability distributions.
 * 
 */
class Distribution {
private:
    std::mt19937 _Gen; //! Random number generator.

public:
    //! Constructor
    Distribution();

    //! Sets the random number generator seed.
    void SetSeed(uint32_t _Seed);

    //! Generates a random sample from the distribution of size numSamples.
    virtual std::vector<float> RandomSample(float loc, float scale, size_t numSamples) = 0;
    
    //! Probability distribution function
    virtual std::vector<float> PDF(std::vector<float> x, float loc, float scale) = 0;
    virtual float PDF(float x, float loc, float scale) = 0;
    
    //! Cumulative distribution function
    virtual std::vector<float> CDF(std::vector<float> x, float loc, float scale) = 0;
    virtual float CDF(float x, float loc, float scale) = 0;

    //! Percent point function.
    virtual std::vector<float> PPF(std::vector<float> x, float loc, float scale) = 0;
    virtual float PPF(float x, float loc, float scale) = 0;
    
    //! Mean, variance, skewness and kurtosis
    virtual std::tuple<float, float, float, float> Stats(float loc, float scale) = 0;

    //! Mean
    virtual float Mean(float loc, float scale) = 0;
    //! Standard deviation
    virtual float Std(float loc, float scale) = 0;
};

}; // Close Namespace Distributions
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
