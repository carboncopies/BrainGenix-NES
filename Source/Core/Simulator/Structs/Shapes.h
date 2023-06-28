//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the shapes struct.
    Additional Notes: None
    Date Created: 2023-06-27
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <variant>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Sphere.h>
#include <Simulator/Structs/Cylinder.h>
#include <Simulator/Structs/Box.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Shapes {


enum ShapeVariantOrder {
    SHAPE_SPHERE=0,
    SHAPE_CYLINDER,
    SHAPE_BOX
};


/**
 * @brief This struct contains the various shapes used in the simulation. It has a vector containing each of the shapes.
 * For now, the "id" of the shape, is simply it's index minus an offset (cylinders are -1000000, boxes -2000000, etc.)
 * 
 */
struct Shapes {

    std::vector<Sphere> Spheres; /**Vector of spheres owned by the simulation.*/
    std::vector<Cylinder> Cylinders; /**Vector of cylinders owned by the simulation*/
    std::vector<Box> Boxes; /**Vector of Boxes owned by the simulation*/

    std::vector<std::variant<Sphere, Cylinder, Box>> Shapes; /**Vector of variants, each being a shape, note that the type order here MUST match the above enum*/

};

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG