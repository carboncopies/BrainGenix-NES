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

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


/**
 * @brief Struct representing a cuboidal box geometry.
 * 
 */
struct Box: Geometry {

    float Dims_um[3] = {5.0, 10.0, 10.0}; //! Dimensions of the box.
    float Rotations_rad[3] = {0.0, 0.0, 0.0}; //! Rotation of the box (euler angles) in radians.
    
    //! Constructors
    Box();  
    Box(float Center_um[3], float Dims_um[3]); 
    Box(float Center_um[3], float Dims_um[3], float Rotations_rad[3]);

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

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
