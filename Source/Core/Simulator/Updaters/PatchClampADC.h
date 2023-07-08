//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the PatchClampADC Updater.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <assert.h>
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/PatchClampADC.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Updater {

/**
 * @brief Resets the target ADC.
 * Clears all recorded data, and resets the list to empty.
 * 
 * @param _Target 
 */
void PatchClampADCReset(Tools::PatchClampADC* _Target, Simulation* _Sim);

/**
 * @brief Updates the target ADC.
 * Will record membrane potnetial data into local vector as needed.
 * 
 * @param _Target 
 */
void PatchClampADCUpdate(Tools::PatchClampADC* _Target, Simulation* _Sim);


}; // Close Namespace Updater
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG