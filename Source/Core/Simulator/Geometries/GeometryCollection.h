//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the GeometryCollection struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <variant>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Box.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


enum GeometryVariantOrder {
    GEOMETRY_SPHERE=0,
    GEOMETRY_CYLINDER,
    GEOMETRY_BOX
};

/**
 * @brief This struct contains the various geometries used in the simulation. It has a vector containing each of the geometries.
 * For now, the "id" of the geometry, is simply its index minus an offset (cylinders are -1000000, boxes -2000000, etc.)
 * 
 */
struct GeometryCollection {

    // std::vector<Sphere> Spheres; /**Vector of spheres owned by the simulation.*/
    // std::vector<Cylinder> Cylinders; /**Vector of cylinders owned by the simulation*/
    // std::vector<Box> Boxes; /**Vector of Boxes owned by the simulation*/

    std::vector<std::variant<Sphere, Cylinder, Box>> Geometries; //! Vector of variants, each being a geometry, note that the type order here MUST match the above enum.

};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
