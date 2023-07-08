//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Box struct.
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
namespace Shapes {


/**
 * @brief Name of the Box
 * 
 */
struct Box {

    std::string Name; /**Name of the Box*/
    int ID; /**ID of the Box*/

    float CenterPos_nm[3]; /**Position of the center of the box*/
    float Scale_nm[3]; /**Scale of the box*/
    float Rotation_rad[3]; /**Rotation of the box (euler angles) in radians*/

};

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG