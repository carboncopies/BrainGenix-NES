//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief Name of the simulation
 * 
 */
struct Simulation {

    std::string Name; /**Name of the simulation*/
    int ID; /**ID of the simulation*/

};


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG