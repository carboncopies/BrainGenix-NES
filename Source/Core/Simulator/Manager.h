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
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/StaticRoutes.h>

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