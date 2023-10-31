//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the TruncNorm class, representing
                 the truncated normal probability distributions.
    Additional Notes: None
    Date Created: 2023-10-10
*/

#pragma once

#include <cassert>
#include <cmath>

#include <Simulator/Distributions/Distribution.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

/**
 * @brief This class represents the truncated normal probability distribution.
 *
 */
class TruncNorm : public Distribution {
  public:
    float a; //! Lower bound of the distribution.
    float b; //! Upper bound of the distribution.

    //! Constructors
    TruncNorm(float _a, float _b);
    TruncNorm(float _a, float _b, float _loc, float _scale);

    //! Generates a random sample from the distribution of size numSamples.
    std::vector<float> RandomSample(size_t numSamples);

    //! Probability distribution function
    std::vector<float> PDF(std::vector<float> x);
    float PDF(float x);

    //! Cumulative distribution function
    std::vector<float> CDF(std::vector<float> x);
    float CDF(float x);

    //! Mean and standard deviation
    std::tuple<float, float> Stats();

    //! Mean
    float Mean();

    //! Standard deviation
    float Std();
};

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
