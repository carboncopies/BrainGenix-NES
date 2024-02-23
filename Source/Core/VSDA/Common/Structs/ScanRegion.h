
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the scan region struct.
    Additional Notes: None
    Date Created: 2023-11-29
    Author(s): Thomas Liao, Randal Koene


    Copyright (C) 2023  Thomas Liao, Randal Koene

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
#include <cstdint>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Defines a bounding box using two points where we wish to scan with the microscope.
 *
 * Note: For purposes of translation, Rotation of the scanregion means rotation of the stage or sample.
 * 
 */
struct ScanRegion {

    /** Point 1*/
    float Point1X_um;
    float Point1Y_um;
    float Point1Z_um;

    /** Point 2*/
    float Point2X_um;
    float Point2Y_um;
    float Point2Z_um;

    /** Sample Rotation */
    float SampleRotationX_rad;
    float SampleRotationY_rad;
    float SampleRotationZ_rad;



    /**
     * @brief Helper function that returns the x,y,z size of this region.
     *
     * @return std::string
     */
    std::string Dimensions();

    /**
     * @brief Returns a string representation of the two points defining this region.
     *
     * @return std::string
     */
    std::string ToString();

    /**
     * @brief Print the voxel size in voxels.
     * 
     * @param _VoxelScale_um 
     * @return std::string 
     */
    std::string GetDimensionsInVoxels(float _VoxelScale_um);


    /**
     * @brief Size helper functions, returns the size in the indicated dimension
     *
     * @return double
     */
    double SizeX();
    double SizeY();
    double SizeZ();

    /**
     * @brief Returns the length of the voxel array needed to store this scan region.
     *
     * @param _VoxelScale
     * @return uint64_t
     */
    uint64_t GetVoxelSize(float _VoxelScale_um);
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
