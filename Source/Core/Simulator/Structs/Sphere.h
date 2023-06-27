//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the sphere struct.
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
 * @brief Name of the Sphere
 * 
 */
struct Sphere {

    std::string Name; /**Name of the Sphere*/
    int ID; /**ID of the Sphere*/

    float Radius_nm; /**Radius in nanometers of the sphere*/
    float Center_nm[3]; /**Position of the center of the sphere in nanometers (relative to origin)*/

};


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG