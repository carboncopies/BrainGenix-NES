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

    std::string Dimensions();
};


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
