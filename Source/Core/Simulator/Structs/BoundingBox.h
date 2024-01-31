//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the bounding box struct.
    Additional Notes: None
    Date Created: 2023-11-29
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <cstdint>
#include <string>

namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Defines a rectangular bounding box between the two points given.
 * Note that point 1 should be smaller than point 2.
 *
 */
struct BoundingBox {
    float bb_point1[3];
    float bb_point2[3];

    /**
     * @brief Prints the dimensions of the box in X,Y,Z
     *
     * @return std::string
     */
    std::string Dimensions();

    /**
     * @brief Prints the two points of the bounding box, X1,Y1,Z1, X2,Y2,Z2
     *
     * @return std::string
     */
    std::string ToString();

    /**
     * @brief Helper that checks if this box intersects with another.
     *
     * @param _Box
     * @return true
     * @return false
     */
    bool IsIntersecting(BoundingBox _Box);

    uint64_t GetVoxelSize(float _VoxelScale_um);
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
