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

#include <Simulator/Distributions/Distribution.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

/**
 * @brief This class represents the truncated normal probability distribution.
 * 
 */
class TruncNorm: public Distribution {
public:
    float a, b;

    //! Constructors
    TruncNorm(float _a, float _b);

    //! Generates a random sample from the distribution of size numSamples.
    std::vector<float> RandomSample(float loc, float scale, size_t numSamples);
    
    //! Probability distribution function
    std::vector<float> PDF(std::vector<float> x, float loc, float scale);
    float PDF(float x, float loc, float scale);
    
    //! Cumulative distribution function
    std::vector<float> CDF(std::vector<float> x, float loc, float scale);
    float CDF(float x, float loc, float scale);

    //! Percent point function.
    std::vector<float> PPF(std::vector<float> x, float loc, float scale);
    float PPF(float x, float loc, float scale);
    
    //! Mean, variance, skewness and kurtosis
    std::tuple<float, float, float, float> Stats(float loc, float scale);

    //! Mean
    float Mean(float loc, float scale);

    //! Standard deviation
    float Std(float loc, float scale);
};

}; // Close Namespace Distributions
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
