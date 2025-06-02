//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the scan region struct.
    Additional Notes: None
    Date Created: 2024-01-22
    Author(s): Thomas Lia


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
// #include <string>
// #include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
// #include <VSDA/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/Common/Structs/ScanRegion.h>

#include <Simulator/Structs/Simulation.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {



/**
 * @brief This struct defines a region within the whole region to be scanned.
 * 
 * We break up entire regions into smaller subregions to handle internally, allowing for arbitrarily large regions.
 * This way, we avoid allocating a huge array on the server.
 */
struct SubRegion {

    // Set Offset Parameters, added to the resulting image filenames

    /** @brief Offset from origin in the x-axis in microns. */
    double RegionOffsetX_um;

    /** @brief Offset from origin in the y-axis in microns. */
    double RegionOffsetY_um;

    // double RegionOffsetZ_um; /**Offset from origin in the z axis in microns*/

    /** 
     * @brief Maximum number of images in the x direction.
     * 
     * Useful for fixing subregion rounding errors.
     */
    int MaxImagesX;

    /** 
     * @brief Maximum number of images in the y direction.
     * 
     * Useful for fixing subregion rounding errors.
     */
    int MaxImagesY;

    /** @brief Layer offset from the bottom of the image stack in microns. */
    size_t LayerOffset;

    // Working Data Params

    /** @brief Region that will be used for rendering. */
    Simulator::ScanRegion Region;

    /** @brief Pointer to the simulation being rendered. */
    Simulator::Simulation* Sim;

    // std::unique_ptr<VoxelArray> RegionArray; /**Array for this region, which we deallocate when we're done with*/
};


}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
