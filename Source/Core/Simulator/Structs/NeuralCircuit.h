//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the NeuralCircuit abstract struct.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#pragma once

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

/**
 * @brief This struct provides the base struct for all neural circuits.
 * 
 */
struct NeuralCircuit {

    int ID; /**ID of the neural circuit */
};

}; // Close Namespace CoreStructs
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
