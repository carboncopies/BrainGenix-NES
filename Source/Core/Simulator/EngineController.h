//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine controller.
    Additional Notes: None
    Date Created: 2023-06-29
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This function is what is the target of the new worker thread, and handles setting up a new engine for this simulation, as well as monitoring it and invoking the util functions as needed.
 * 
 */
void SimulationEngineThread(Simulation* _Sim);



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG