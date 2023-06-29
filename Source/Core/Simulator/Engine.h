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
#include <assert.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <Simulator/Updaters/PatchClampADC.h>
#include <Simulator/Updaters/PatchClampDAC.h>
#include <Simulator/Updaters/Receptor.h>
#include <Simulator/Updaters/Staple.h>


namespace BG {
namespace NES {
namespace Simulator {



/**
 * @brief This class provides the infrastructure to run simulations.
 */
class Engine {

private:

public:

    Engine();

    ~Engine();


    /**
     * @brief Resets all attributes of the given simulation which need to be cleared, (this includes any recorded data).
     * 
     */
    void Reset(Simulation* _Sim);

    /**
     * @brief Runs the simulation for the amount of time specified in the simulation's struct.
     * 
     */
    void RunFor(Simulation* _Sim);


};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG