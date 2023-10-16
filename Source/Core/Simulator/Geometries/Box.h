//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Box struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

#include <vector>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

/**
 * @brief Struct representing a cuboidal box geometry.
 *
 */
struct Box : Geometry {

    Vec3D Dims_um{5.0, 10.0, 10.0}; //! Dimensions of the box.
    Vec3D Rotations_rad{0.0, 0.0,
                        0.0}; //! Rotation of the box (euler angles) in radians.

    //! Constructors
    Box();
    Box(Vec3D _Center_um, Vec3D _Dims_um);
    Box(Vec3D _Center_um, Vec3D _Dims_um, Vec3D _Rotations_rad);

    //! Renders the box in 3D.
    void Show();

    //! Returns the volume of the box in micrometer^3.
    float Volume_um3();

    //! Returns 1 of n equally sliced subpartitions of the Box
    //! shape, when lined up from left to right along the width.
    std::vector<std::vector<float>> EqualSliceBounds(int nSlices, int slice);

    //! Returns a vector of dimensions of the Box.
    std::vector<float> Sides();
};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
