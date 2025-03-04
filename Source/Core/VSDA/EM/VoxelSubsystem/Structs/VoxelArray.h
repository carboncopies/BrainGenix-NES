
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
#include <atomic>


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>

#include <VSDA/Common/Structs/ScanRegion.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {


enum VoxelState:uint8_t {
    VoxelState_EMPTY=0,
    VoxelState_INTERIOR=1,
    VoxelState_BORDER=2,
    VoxelState_BLACK=3,
    VoxelState_WHITE=4,
    VoxelState_OUT_OF_BOUNDS=5
};



struct VoxelType {

    // uint8_t Intensity_; /**Value from 0-255 representing the intensity (brightness) of this voxel, done during coloring stage of compositing*/
    VoxelState State_; /**Determine if this voxel is near the edge of a shape or not*/
    uint8_t DistanceToEdge_vox_; /**Determines the distance to the nearest edge (in voxels), used to calculate borders later on, set during rasterization stage of compositing.*/

    uint64_t ParentUID; /**Parent ID Of the voxel, indicates the id of the neuron it belongs to*/

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

    BoundingBox BoundingBox_; /**Set the bounding box of this voxel array (relative to the simulation orign), used by subregions*/

    BG::Common::Logger::LoggingSystem* Logger_; /**Instance of logging system*/


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
    VoxelArray(BG::Common::Logger::LoggingSystem* _Logger, BoundingBox _BB, float _VoxelScale_um);
    VoxelArray(BG::Common::Logger::LoggingSystem* _Logger, ScanRegion _Region, float _VoxelScale_um);

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
    VoxelType GetVoxel(int _X, int _Y, int _Z);


    /**
     * @brief Sets the voxel at the given coords to _Value.
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @param _Value 
     */
    void SetVoxel(int _X, int _Y, int _Z, VoxelType _Value);
    void SetVoxelAtIndex(int _XIndex, int _YIndex, int _ZIndex, VoxelType _Value);

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
     * Returns the position at the current indexes.
     */
    Geometries::Vec3D GetPositionAtIndex(int _XIndex, int _YIndex, int _ZIndex);



    /**
     * @brief Sets the voxel if the one currently there is not darker.
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @param _Value 
     */
    // void SetVoxelIfNotDarker(float _X, float _Y, float _Z, VoxelType _Value);
    // void SetVoxelIfNotDarkerAtIndex(int _X, int _Y, int _Z, VoxelType _Value);

    /**
     * @brief Compositor function that simply sets information about the sate of the given voxel.
     */
    void CompositeVoxel(float _X, float _Y, float _Z, VoxelState _State, float _DistanceToEdge, uint64_t _ParentUID);
    void CompositeVoxelAtIndex(int _X, int _Y, int _Z, VoxelState _State, float _DistanceToEdge, uint64_t _ParentUID);

    /**
     * @brief Get the size of the array, populate the int ptrs
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     */
    void GetSize(int* _X, int* _Y, int* _Z);

    /**
     * @brief Attempt to set the size of the current array, if it's less than or equal to the max array size.
     * 
     * @param _X Dimension In Voxels
     * @param _Y Dimension In Voxels
     * @param _Z Dimension In Voxels
     */
    bool SetSize(int _X, int _Y, int _Z);
    bool SetSize(ScanRegion _TargetSize, float _VoxelScale_um);

    /**
     * @brief Update the given bounding box with the new size.
     * 
     * @param _NewBoundingBox 
     * @return true 
     * @return false 
     */
    bool SetBB(BoundingBox _NewBoundingBox);
    bool SetBB(ScanRegion _NewBoundingBox);

    /**
     * @brief Checks if the given x value is in the x range.
     * 
     * @param _X 
     * @return true 
     * @return false 
     */
    bool IsInRangeX(float _X);

    /**
     * @brief Checks if the given y value is in the y range.
     * 
     * @param _y 
     * @return true 
     * @return false 
     */
    bool IsInRangeY(float _Y);

    /**
     * @brief Checks if the given z value is in the z range.
     * 
     * @param _Z 
     * @return true 
     * @return false 
     */
    bool IsInRangeZ(float _Z);

    /**
     * @brief Get the x dimensions
     * 
     * @return int 
     */
    int GetX();

    int GetXIndexAtPosition(float _X_Worldspace_um);

    float GetXPositionAtIndex(int _XIndex);

    /**
     * @brief Get the y dimensions
     * 
     * @return int 
     */
    int GetY();

    int GetYIndexAtPosition(float _X_Worldspace_um);

    float GetYPositionAtIndex(int _YIndex);


    /**
     * @brief Get the Z dimensions
     * 
     * @return int 
     */
    int GetZ();

    int GetZIndexAtPosition(float _X_Worldspace_um);

    float GetZPositionAtIndex(int _ZIndex);

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
    BoundingBox GetBoundingBox();


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


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
