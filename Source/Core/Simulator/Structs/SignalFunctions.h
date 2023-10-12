//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides functions for creating some fundamental signals using basic mathematical functions.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

#include <string>
#include <cmath>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {

float DoubleExponentExpr(float amp, float tauRise, float tauDecay, float tDiff);

}; // Close Namespace 
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
