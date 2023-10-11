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
    TruncNorm(float _a, float _b, float _loc, float _scale);

    //! Generates a random sample from the distribution of size numSamples.
    std::vector<float> RandomSample(size_t numSamples);
    
    //! Probability distribution function
    std::vector<float> PDF(std::vector<float> x);
    float PDF(float x);
    
    //! Cumulative distribution function
    std::vector<float> CDF(std::vector<float> x);
    float CDF(float x);

    //! Percent point function.
    std::vector<float> PPF(std::vector<float> x);
    float PPF(float x);
    
    //! Mean, variance, skewness and kurtosis
    std::tuple<float, float, float, float> Stats();

    //! Mean
    float Mean();

    //! Standard deviation
    float Std();
};

}; // Close Namespace Distributions
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
