//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the PatchClampDAC struct.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

/**
 * @brief This struct implements the storage state of the DACs.
 * 
 */
struct PatchClampDAC {

    std::string Name; /**Name of the DAC.*/
    int ID; /**ID of the DAC.*/

    int DestinationCompartmentID; /**DestinationCompartmentID ID of the compartment receiving DAC output.*/

    float ClampPos_nm[3]; /**Position of the clamp in world space coordinates*/
    float Timestep_ms; /**Simulation time for which each step in the output voltages list is replayed.*/

    std::vector<float> DACVoltages_mV; /**List of voltages to be replayed by the DAC.*/

};

}; // Close Namespace Tools
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG