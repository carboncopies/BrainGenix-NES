//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides functions for creating some fundamental
   signals using basic mathematical functions.

   Additional Notes: None

   Date
   Created: 2023-06-28
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {

//! Double exponential expression used in calculation of post-synaptic potential
//! contributions.
float DoubleExponentExpr(float amp, float tauRise, float tauDecay, float tDiff);
//! One-dimensional convolution.
std::vector<float> Convolve1D(std::vector<float> signal, std::vector<float> kernel);

}; // namespace SignalFunctions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
