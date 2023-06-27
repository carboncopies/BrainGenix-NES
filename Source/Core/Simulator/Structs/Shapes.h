//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the shapes struct.
    Additional Notes: None
    Date Created: 2023-06-27
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Sphere.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Shapes {


/**
 * @brief Name of the Sphere
 * 
 */
struct Shapes {

    std::vector<Sphere> Spheres; /**Vector of spheres owned by the simulation.*/

};

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG