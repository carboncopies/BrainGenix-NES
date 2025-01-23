//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the sphere struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

#include <sstream>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Geometries/Geometry.h>

#include <VSDA/Common/Structs/WorldInfo.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


/**
 * Just the easily storable fixed-size data.
 */
struct SphereBase: Geometry {
    float Radius_um; //! Radius in micrometers of the sphere.
    uint64_t ParentID; // ID of the parent object.


    //! --- The folllwing is here due to pure virtual functions in Geometry.
    //! Returns the volume of the sphere in micrometer^3.
    float Volume_um3();

    //! Returns the bounding box
    virtual BoundingBox GetBoundingBox(VSDA::WorldInfo& _WorldInfo);
    virtual bool IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo);
    virtual bool IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo);

    std::string str() const {
        std::stringstream ss;
        ss << Geometry::str();
        ss << "Radius_um: " << Radius_um << '\n';
        return ss.str();
    }
};

/**
 * @brief This struct defines a sphere geometry used in creation of components of a simple ball-and-stick neural circuit.
 * 
 */
struct Sphere: SphereBase {

    std::string Name;

    //! Constructors
    Sphere();
    Sphere(const SphereBase& _Base): SphereBase(_Base) {}
    Sphere(float _Radius_um);
    Sphere(const Vec3D & _Center_um, float _Radius_um);

    //! Renders the sphere in 3D.
    void Show(); 

};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
