
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

public:

    VoxelType*** Data_;

    int SizeX_; /**Number of voxels in x dimension*/
    int SizeY_; /**Number of voxels in y dimension*/
    int SizeZ_; /**Number of voxels in z dimension*/

    /**
     * @brief Construct a new Voxel Array object
     * 
     * @param _SizeX X dimension (in voxels)
     * @param _SizeY Y dimension (in voxels)
     * @param _SizeZ Z dimension (in voxels)
     */
    VoxelArray(int _SizeX, int _SizeY, int _SizeZ);

    ~VoxelArray();

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
