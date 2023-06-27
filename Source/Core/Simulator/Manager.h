//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the main simulation manager code.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation/Simulation.h>

#include <Config/Config.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This class provides the infrastructure to run simulations.
 */
class Manager {

private:
    Config::Config* Config_; /**Pointer to configuration struct owned by rest of system*/

    std::vector<std::unique_ptr<Simulation>> Simulations_; /**Vector containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/
    // Note: This simulation vector is not thread safe and will probably segfault if you try to multithread this
    // we will fix this later when we scale the system (DO NOT ALLOW RPC to use more than 1 thread unless this is fixed!)

public:

    /**
     * @brief Construct a new Manager object
     * Give this a pointer to an initialized configuration object.
     * 
     * @param _Config 
     */
    Manager(Config::Config* _Config);


    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();


};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG