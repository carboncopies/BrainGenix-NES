//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-19
    Author(s): Thomas Liao


    Copyright (C) 2024  Thomas Liao

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
#include <vector>
#include <memory>
#include <atomic>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


/**
 * @brief Structure that defines the work to be completed.
 * 
 * This involves taking a pointer to the voxel array in question,
 * reading the voxels specified, and generating an image with some extra parameters.
 * This is done with multiple threads, but since we're just reading from the voxel array,
 * no synchronization is needed for accessing this data.
 */
struct ProcessingTask {

    /** @brief Width of this image in pixels. */
    int Width_px;

    /** @brief Height of this image in pixels. */
    int Height_px;

    /** @brief Starting x index of the region. */
    int VoxelStartingX;

    /** @brief Starting y index of the region. */
    int VoxelStartingY;

    /** @brief Ending x index of the region. */
    int VoxelEndingX;

    /** @brief Ending y index of the region. */
    int VoxelEndingY;

    /** @brief Slice number that we're processing. */
    int VoxelZ;

    /** @brief Indicates if this task has been processed or not. */
    std::atomic_bool IsDone_ = false;

    /** @brief Filename that this image is to be written to. */
    std::string TargetFileName_;

    /** @brief Directory path where the image is to be written to. */
    std::string TargetDirectory_;

    /** @brief Pointer to the voxel array that we're rendering from. */
    VoxelArray* Array_;

    /** 
     * @brief Pointer to a vector containing all the calcium concentrations.
     * 
     * Each index corresponds to a specific region in the voxel array.
     */
    std::vector<std::vector<float>>* CalciumConcentrationByIndex_;

    /** @brief Index of the current timestep that we're processing. */
    int CurrentTimestepIndex_;

    /** @brief Brightness amplification factor for rendering. */
    float BrightnessAmplification;

    /** @brief Attenuation per micrometer for rendering. */
    float AttenuationPerUm;

    /** @brief Resolution of each voxel in micrometers. */
    float VoxelResolution_um;

    /** @brief Number of voxels per slice in the z-dimension. */
    int NumVoxelsPerSlice;
};


}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
