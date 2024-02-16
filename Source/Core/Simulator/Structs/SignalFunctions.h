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
#include <deque>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {

//! Double exponential expression used in calculation of post-synaptic potential
//! contributions.
float DoubleExponentExpr(float amp, float tauRise, float tauDecay, float tDiff);

//! One-dimensional convolution.
//! For the sake of efficiency, the convolved vector must already have been
//! prepared to the size: signal.size()+kernel.size()-1.
//! Also, for the sake of efficiency, we store and provide the already reversed
//! kernel instead of reversing it here each time.
bool Convolve1D(const std::deque<float> & signal, const std::vector<float> & reversed_kernel, std::vector<float> & convolved);

}; // namespace SignalFunctions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
