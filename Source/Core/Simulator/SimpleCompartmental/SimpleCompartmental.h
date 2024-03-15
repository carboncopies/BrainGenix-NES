//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the SimpleCompartmental struct,
                 representing a basic building block for a 
                 SimpleCompartmental neural circuit.
    Additional Notes: None
    Date Created: 2024-03-11
*/

#pragma once

#include <Simulator/BallAndStick/BSNeuron.h>

namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Provides object data and member functions of the SimpleCompartmental
 *        neuron class.
 */
class SimpleCompartmental: BallAndStick::BSNeuron {
public:
    CoreStructs::SCNeuronStruct build_data; // Copy of the struct that was used to build this neuron.

public:
    SimpleCompartmental(int ID, std::unordered_map<std::string, Geometries::Geometry*> & morphology);
    SimpleCompartmental(const CoreStructs::SCNeuronStruct & scneuronstruct);
};

} // Simulator
} // NES
} // BG
