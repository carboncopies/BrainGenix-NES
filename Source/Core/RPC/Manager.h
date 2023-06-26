//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <functional>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/Config.h>


namespace BG {
namespace NES {
namespace API {


/**
 * @brief This class manages the NES RPC Server instance as well as adding/removing/etc routes on said RPC server.
 * 
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

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG