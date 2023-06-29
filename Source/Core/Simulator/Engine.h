//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine code.
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
 * @brief This class provides the infrastructure to run simulations.
 */
class Engine {

private:

    Simulation* Sim_; /**Local copy of pointer to simulation. We assume that this pointer will *never* be invalidated.*/

public:

    Engine(Simulation* _Sim);

    ~Engine();




};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG