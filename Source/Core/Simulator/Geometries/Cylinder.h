//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the cylinder struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

#include <cmath>
#include <tuple>

#include <glm/mat3x3.hpp>

#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

/**
 * @brief This struct defines a cylinder geometry used in creation of components
 * of simple ball-and-stick neural circuits.
 *
 */
struct Cylinder : Geometry {

    float End0Radius_um =
        1.0; //! Radius in micrometers of the first end of the cylinder.
    Vec3D End0Pos_um; //! Position of the first end of the Cylinder in
                      //! micrometers (relative to origin).
    float End1Radius_um =
        1.0; //! Radius in micrometers of the second end of the cylinder.
    Vec3D End1Pos_um{1.0, 0.0,
                     0.0}; //! Position of the second end of the Cylinder in
                           //! micrometers (relative to origin).

    // Constructors
    Cylinder();
    Cylinder(float _End0Radius_um,
             BG::NES::Simulator::Geometries::Vec3D _End0Pos_um,
             float _End1Radius_um,
             BG::NES::Simulator::Geometries::Vec3D _End1Pos_um);

    //! Renders the cylinder in 3D.
    void Show();

    //! Returns the volume of the cylinder in micrometer^3.
    float Volume_um3();

    //! Returns the distance from the origin along cylinder axis at specified
    //! fraction of height distance from end 0.
    float RAtPosition_um(float position);

    //! Gets the rotation in radians with respect to the x, y and z axes
    std::tuple<float, float, float> GetRotation_rad();

    //! Returns the bounding box
    virtual BoundingBox GetBoundingBox();
};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
