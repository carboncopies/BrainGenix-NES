//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Generic random distribution class,
                 that provides int and float uniform or normal
                 probability distributions.
    Additional Notes: None
    Date Created: 2024-02-21
*/

#pragma once

#include <cassert>
#include <cmath>

#include <random>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

/**
 * @brief This class represents the generic probability distribution.
 *
 */
class Generic {
  protected:
    std::mt19937 Gen_; //! Random number generator.

  public:
    //! Constructors
    Generic(uint32_t Seed) { Gen_.seed(Seed); }

    int UniformRandomInt();

    /**
     * @brief Generates a random sample from the uniform distribution.
     * 
     * @return Returns a single value from the uniform distribution.
     */
    float UniformRandomFloat();

    /**
     * @brief Generates a random sample from the normal distribution.
     * 
     * @return Returns a single value from the normal distribution.
     */
    float NormalRandomFloat();

};

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
