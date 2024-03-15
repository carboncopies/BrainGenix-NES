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

namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Provides object data and member functions of the SimpleCompartmental
 *        neuron class.
 */
class SimpleCompartmental: BallAndStick::BSNeuron {
public:
    SimpleCompartmental(int ID, std::unordered_map<std::string, Geometries::Geometry*> & morphology);
};

} // Simulator
} // NES
} // BG
