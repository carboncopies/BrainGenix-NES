//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the cylinder struct.
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
 * @brief Name of the Cylinder
 * 
 */
struct Cylinder {

    std::string Name; /**Name of the Cylinder*/
    int ID; /**ID of the Cylinder*/

    float Point1Radius_nm; /**Radius in nanometers of the first point of the cylinder*/
    float Point1Pos_nm[3]; /**Position of the first point of the Cylinder in nanometers (relative to origin)*/
    float Point2Radius_nm; /**Radius in nanometers of the second point of the cylinder*/
    float Point2Pos_nm[3]; /**Position of the second point of the Cylinder in nanometers (relative to origin)*/

};

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG