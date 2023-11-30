
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


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {


typedef uint8_t VoxelType;

enum VoxelState {
    EMPTY=0,
    FILLED=1
};


/**
 * @brief Defines the voxel array.
 * 
 */
class VoxelArray {

private:

    VoxelType* Data_; /**Big blob of memory that holds all the voxels*/

    int SizeX_; /**Number of voxels in x dimension*/
    int SizeY_; /**Number of voxels in y dimension*/
    int SizeZ_; /**Number of voxels in z dimension*/


    /**
     * @brief Returns the flat index for the voxel at the given coords.
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @return int 
     */
    int GetIndex(int _X, int _Y, int _Z);



public:

    /**
     * @brief Construct a new Voxel Array object
     * 
     * @param _SizeX X dimension (in voxels)
     * @param _SizeY Y dimension (in voxels)
     * @param _SizeZ Z dimension (in voxels)
     */
    VoxelArray(int _SizeX, int _SizeY, int _SizeZ);

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

    /**
     * @brief Set the Voxel At the given Position (using the given scale) to the given value.
     * Converts the given float x,y,z um position to index, then calls setvoxel normally
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     * @param _VoxelScale 
     */
    void SetVoxelAtPosition(float _X, float _Y, float _Z, float _VoxelScale, VoxelType _Value);

    /**
     * @brief Get the size of the array, populate the int ptrs
     * 
     * @param _X 
     * @param _Y 
     * @param _Z 
     */
    void GetSize(int* _X, int* _Y, int* _Z);

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
     * @brief Clears the given array to all 0s
     * 
     */
    void ClearArray();

};


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
