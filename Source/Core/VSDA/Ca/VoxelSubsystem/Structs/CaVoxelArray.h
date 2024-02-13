
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



struct VoxelType {

    bool IsFilled_ = false;
    size_t CompartmentID_ = 0;
    bool IsBorder_ = false;

};





/**
 * @brief Defines the voxel array.
 * 
 */
class VoxelArray {

private:

    std::unique_ptr<VoxelType[]> Data_; /**Big blob of memory that holds all the voxels*/
    uint64_t DataMaxLength_ = 0;

    uint64_t SizeX_; /**Number of voxels in x dimension*/
    uint64_t SizeY_; /**Number of voxels in y dimension*/
    uint64_t SizeZ_; /**Number of voxels in z dimension*/

    float VoxelScale_um; /**Set the size of each voxel in micrometers*/

    Simulator::BoundingBox BoundingBox_; /**Set the bounding box of this voxel array (relative to the simulation orign)*/



    /**
     * @brief Returns the flat index for the voxel at the given coords.
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @return uint64_t 
     */
    uint64_t GetIndex(int _X, int _Y, int _Z);



public:

    /**
     * @brief Construct a new Voxel Array object
     * 
     * @param _BB Bounding box of the array, in world space
     * @param _VoxelScale_um Scale of each voxel in micrometers
     */
    VoxelArray(Simulator::BoundingBox _BB, float _VoxelScale_um);
    VoxelArray(Simulator::ScanRegion _Region, float _VoxelScale_um);

    /**
     * @brief Destroy the Voxel Array object
     * 
     */
    ~VoxelArray();



    /**
     * @brief Returns the voxel at the given coordinates
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @return VoxelType 
     */
    VoxelType GetVoxel(int _X, int _Y, int _Z, bool* _Status = nullptr);


    /**
     * @brief Sets the voxel at the given coords to _Value.
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @param _Value 
     */
    void SetVoxel(int _X, int _Y, int _Z, VoxelType _Value);

    /**
     * @brief Set the Voxel At the given Position (using the given scale) to the given value.
     * Converts the given float x,y,z um position to index, then calls setvoxel normally
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @param _Value
     */
    void SetVoxelAtPosition(float _X, float _Y, float _Z, VoxelType _Value);

    /**
     * @brief Get the size of the array, populate the int ptrs
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     */
    void GetSize(int* _X, int* _Y, int* _Z);

    /**
     * @brief Update the given bounding box with the new size.
     * 
     * @param _NewBoundingBox 
     * @return true 
     * @return false 
     */
    bool SetBB(Simulator::BoundingBox _NewBoundingBox);
    bool SetBB(Simulator::ScanRegion _NewBoundingBox);

    /**
     * @brief Get the x dimensions
     * 
     * @return int 
     */
    int GetX();

    /**
     * @brief Get the y dimensions
     * 
     * @return int 
     */
    int GetY();

    /**
     * @brief Get the Z dimensions
     * 
     * @return int 
     */
    int GetZ();

    /**
     * @brief Returns the resolution of the given object in micrometers.
     * 
     * @return float 
     */
    float GetResolution();

    /**
     * @brief Returns the bounding box of this voxel array (in simulation world space).
     * 
     * @return BoundingBox 
     */
    Simulator::BoundingBox GetBoundingBox();


    /**
     * @brief Clears the given array to all 0s
     * 
     */
    void ClearArray();
    void ClearArrayThreaded(int _NumThreads=10);

    /**
     * @brief Returns the size of the array.
     * 
     * @return uint64_t 
     */
    uint64_t GetSize();

};


}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
