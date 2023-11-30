//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Geometry base struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/BoundingBox.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


/**
 * @brief This abstract struct defines the general attributes of a geometrical shape. * 
 * Geometries representing different geometrical shapes will inherit from this struct.
 */

struct Geometry {
    std::string Name;   //! Name of the Geometry
    int ID;             //! ID of the Geometry

    Vec3D Center_um; //! Center of the geometry in micrometers.

   
    //! Renders the object in 3D.
    virtual void Show() = 0; 
    
    //! Returns the volume of the object in micrometer^3.
    virtual float Volume_um3() = 0;

    //! Gets the bounding box for the given shape
    virtual BoundingBox GetBoundingBox() = 0;

};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
