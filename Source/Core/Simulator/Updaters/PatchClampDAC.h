//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the PatchClampDAC Updater.
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
#include <Simulator/Structs/PatchClampDAC.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Updater {


/**
 * @brief Resets the target DAC.
 * 
 * @param _Target 
 */
void PatchClampDACReset(Tools::PatchClampDAC* _Target, Simulation* _Sim);


/**
 * @brief Updates the target DAC.
 * 
 * @param _Target 
 */
void PatchClampDACUpdate(Tools::PatchClampDAC* _Target, Simulation* _Sim);



}; // Close Namespace Updater
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG