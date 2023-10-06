//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Box struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


/**
 * @brief Struct representing a cuboidal box geometry.
 * 
 */
struct Box: Geometry {

    float Dims_um[3]; //! Dimensions of the box.
    float Rotations_rad[3]; //! Rotation of the box (euler angles) in radians.
    
    //! Renders the box in 3D.
    void Show(); 
    
    //! Returns the volume of the box in micrometer^3.
    float Volume_um3();
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
