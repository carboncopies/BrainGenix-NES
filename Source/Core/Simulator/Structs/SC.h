//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the SC struct.
    Additional Notes: None
    Date Created: 2024-03-15
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Structs/BS.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

/**
 * @brief This struct provides the data storage for each of the compartments
 * 
 */
struct SC: public BS {

};

}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
