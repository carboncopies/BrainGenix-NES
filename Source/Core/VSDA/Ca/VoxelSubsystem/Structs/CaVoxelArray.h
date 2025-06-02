//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the struct used to store the voxel array.
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
#include <inttypes.h>
#include <math.h>
#include <memory>


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>

#include <VSDA/Common/Structs/ScanRegion.h>



namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {

/**
 * @brief Represents a single voxel in the voxel array.
 */
struct VoxelType {
    bool IsFilled_ = false; /**< Indicates whether the voxel is filled. */
    size_t CompartmentID_ = 0; /**< ID of the compartment associated with the voxel. */
    bool IsBorder_ = false; /**< Indicates whether the voxel is on the border. */
};

/**
 * @brief Defines the voxel array used for simulation.
 * 
 * The VoxelArray class provides functionality to manage a 3D array of voxels,
 * including setting and retrieving voxel values, resizing the array, and clearing it.
 */
class VoxelArray {

private:
    std::unique_ptr<VoxelType[]> Data_; /**< Memory block holding all the voxels. */
    uint64_t DataMaxLength_ = 0; /**< Maximum length of the data array. */

    uint64_t SizeX_; /**< Number of voxels in the x dimension. */
    uint64_t SizeY_; /**< Number of voxels in the y dimension. */
    uint64_t SizeZ_; /**< Number of voxels in the z dimension. */

    float VoxelScale_um; /**< Size of each voxel in micrometers. */

    Simulator::BoundingBox BoundingBox_; /**< Bounding box of the voxel array in simulation space. */

    /**
     * @brief Returns the flat index for the voxel at the given coordinates.
     * 
     * @param _X X-coordinate of the voxel.
     * @param _Y Y-coordinate of the voxel.
     * @param _Z Z-coordinate of the voxel.
     * @return uint64_t Flat index of the voxel.
     */
    uint64_t GetIndex(int _X, int _Y, int _Z);

public:
    /**
     * @brief Constructs a new VoxelArray object.
     * 
     * @param _BB Bounding box of the array in world space.
     * @param _VoxelScale_um Scale of each voxel in micrometers.
     */
    VoxelArray(Simulator::BoundingBox _BB, float _VoxelScale_um);

    /**
     * @brief Constructs a new VoxelArray object using a scan region.
     * 
     * @param _Region Scan region defining the array bounds.
     * @param _VoxelScale_um Scale of each voxel in micrometers.
     */
    VoxelArray(Simulator::ScanRegion _Region, float _VoxelScale_um);

    /**
     * @brief Destroys the VoxelArray object.
     */
    ~VoxelArray();

    /**
     * @brief Returns the voxel at the given coordinates.
     * 
     * @param _X X-coordinate of the voxel.
     * @param _Y Y-coordinate of the voxel.
     * @param _Z Z-coordinate of the voxel.
     * @param _Status Optional pointer to store the status of the operation.
     * @return VoxelType The voxel at the specified coordinates.
     */
    VoxelType GetVoxel(int _X, int _Y, int _Z, bool* _Status = nullptr);

    /**
     * @brief Sets the voxel at the given coordinates to the specified value.
     * 
     * @param _X X-coordinate of the voxel.
     * @param _Y Y-coordinate of the voxel.
     * @param _Z Z-coordinate of the voxel.
     * @param _Value The value to set for the voxel.
     */
    void SetVoxel(int _X, int _Y, int _Z, VoxelType _Value);

    /**
     * @brief Sets the voxel at the given position (in micrometers) to the specified value.
     * 
     * Converts the given float x, y, z position to an index, then sets the voxel.
     * 
     * @param _X X-position in micrometers.
     * @param _Y Y-position in micrometers.
     * @param _Z Z-position in micrometers.
     * @param _Value The value to set for the voxel.
     */
    void SetVoxelAtPosition(float _X, float _Y, float _Z, VoxelType _Value);

    /**
     * @brief Retrieves the size of the array in each dimension.
     * 
     * @param _X Pointer to store the size in the x dimension.
     * @param _Y Pointer to store the size in the y dimension.
     * @param _Z Pointer to store the size in the z dimension.
     */
    void GetSize(int* _X, int* _Y, int* _Z);

    /**
     * @brief Updates the bounding box of the voxel array.
     * 
     * @param _NewBoundingBox The new bounding box.
     * @return true If the bounding box was successfully updated.
     * @return false If the update failed.
     */
    bool SetBB(Simulator::BoundingBox _NewBoundingBox);

    /**
     * @brief Updates the bounding box of the voxel array using a scan region.
     * 
     * @param _NewBoundingBox The new scan region.
     * @return true If the bounding box was successfully updated.
     * @return false If the update failed.
     */
    bool SetBB(Simulator::ScanRegion _NewBoundingBox);

    /**
     * @brief Returns the size of the array in the x dimension.
     * 
     * @return int Size in the x dimension.
     */
    int GetX();

    /**
     * @brief Returns the size of the array in the y dimension.
     * 
     * @return int Size in the y dimension.
     */
    int GetY();

    /**
     * @brief Returns the size of the array in the z dimension.
     * 
     * @return int Size in the z dimension.
     */
    int GetZ();

    /**
     * @brief Returns the resolution of the voxel array in micrometers.
     * 
     * @return float Resolution in micrometers.
     */
    float GetResolution();

    /**
     * @brief Returns the bounding box of the voxel array in simulation world space.
     * 
     * @return Simulator::BoundingBox The bounding box.
     */
    Simulator::BoundingBox GetBoundingBox();

    /**
     * @brief Clears the voxel array, setting all values to 0.
     */
    void ClearArray();

    /**
     * @brief Clears the voxel array using multiple threads.
     * 
     * @param _NumThreads Number of threads to use for clearing.
     */
    void ClearArrayThreaded(int _NumThreads = 10);

    /**
     * @brief Returns the total size of the voxel array.
     * 
     * @return uint64_t Total size of the array.
     */
    uint64_t GetSize();
};

}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace NES
}; // Close Namespace BG
