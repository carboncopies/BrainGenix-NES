//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the sphere struct.
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
 * @brief This struct defines a sphere geometry used in creation of components of a simple ball-and-stick neural circuit.
 * 
 */
struct Sphere: Geometry {
    
    float Radius_um; //! Radius in micrometers of the sphere.

    //! Renders the sphere in 3D.
    void Show(); 
    
    //! Returns the volume of the sphere in micrometer^3.
    float Volume_um3();
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG