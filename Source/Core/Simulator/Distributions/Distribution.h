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

#include <chrono>
#include <cstdint>
#include <random>
#include <tuple>
#include <vector>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

/**
 * @brief This struct provides the base class for all continuous probability
 * distributions.
 *
 */
class Distribution {
  protected:
    std::mt19937 _Gen; //! Random number generator.

  public:
    float loc = 0.0;   //! Center of the distribution.
    float scale = 1.0; //! Multiplier to scale the distribution.

    //! Constructor
    Distribution();

    //! Sets the random number generator seed.
    void SetSeed(uint32_t _Seed);

    //! Generates a random sample from the distribution of size numSamples.
    virtual std::vector<float> RandomSample(size_t numSamples) = 0;

    //! Probability distribution function
    virtual std::vector<float> PDF(std::vector<float> x) = 0;
    virtual float PDF(float x) = 0;

    //! Cumulative distribution function
    virtual std::vector<float> CDF(std::vector<float> x) = 0;
    virtual float CDF(float x) = 0;

    //! Mean, variance
    virtual std::tuple<float, float> Stats() = 0;

    //! Mean
    virtual float Mean() = 0;

    //! Standard deviation
    virtual float Std() = 0;
};

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
