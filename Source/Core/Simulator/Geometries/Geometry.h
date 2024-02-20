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

#include <VSDA/Common/Structs/WorldInfo.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

enum GeometryShapeEnum {
    GeometryUnspecified = 0,
    GeometrySphere,
    GeometryCylinder,
    GeometryBox,
    NUMshapes
};

/**
 * @brief This abstract struct defines the general attributes of a geometrical shape. * 
 * Geometries representing different geometrical shapes will inherit from this struct.
 */

struct Geometry {
    std::string Name;   //! Name of the Geometry
    int ID;             //! ID of the Geometry
    GeometryShapeEnum GeometryShape = GeometryUnspecified; //! Shape of the derived class instantiation that inherits Geometry.

    Vec3D Center_um; //! Center of the geometry in micrometers.

   
    //! Renders the object in 3D.
    virtual void Show() = 0; 
    
    //! Returns the volume of the object in micrometer^3.
    virtual float Volume_um3() = 0;

    //! Gets the bounding box for the given shape
    virtual BoundingBox GetBoundingBox(VSDA::WorldInfo& _WorldInfo) = 0;

    /**
     * @brief Determines if this shape is intersecting with the given region.
     * May sometimes give false positives, but we just need it to be really quick and dirty.
     * 
     * @param _Region 
     * @return true 
     * @return false 
     */
    virtual bool IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo) = 0;

    /**
     * @brief Checks if the given world space position is in this shape.
     * 
     * @param _Position_um Position (in world space) to check if is in this shape.
     * @return true Is in shape
     * @return false Is not in shape
     */
    virtual bool IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo) = 0;

};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
