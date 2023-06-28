//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BS struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

/**
 * @brief This struct provides the data storage for each of the compartments
 * 
 */
struct BS {

    std::string Name; /**Name of the BS Compartment*/
    int ID; /**ID of the BS compartment*/

    int ShapeID; /**ID of the associated shape of this compartment*/

    float MembranePotential_mV; /**Membrane potential in millivolts*/
    float SpikeThreshold_mV; /**Action potential spike threshold voltage in millivolts*/
    float DecayTime_ms; /**After hyperpolarization time constant in ms, also known as tau_ahp*/

};

}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG