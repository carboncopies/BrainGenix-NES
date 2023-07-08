//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Staple Updater.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <assert.h>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/Staple.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Updater {


/**
 * @brief Resets the target Staple.
 * 
 * @param _Target 
 */
void StapleReset(Connections::Staple* _Target, Simulation* _Sim);


/**
 * @brief Updates the target Staple.
 * 
 * @param _Target 
 */
void StapleUpdate(Connections::Staple* _Target, Simulation* _Sim);



}; // Close Namespace Updater
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG