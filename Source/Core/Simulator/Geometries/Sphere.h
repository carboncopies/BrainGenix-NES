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
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Geometries/Geometry.h>

#include <VSDA/Common/Structs/WorldInfo.h>


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

    //! Constructors
    Sphere();
    Sphere(float _Radius_um);
    Sphere(const Vec3D & _Center_um, float _Radius_um);

    //! Renders the sphere in 3D.
    void Show(); 
    
    //! Returns the volume of the sphere in micrometer^3.
    float Volume_um3();

    //! Returns the bounding box
    virtual BoundingBox GetBoundingBox(VSDA::WorldInfo& _WorldInfo);
    virtual bool IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo);
    virtual bool IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo);

};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
