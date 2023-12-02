
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines what is imaged and how it's imaged.
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


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <SliceGenerator/Structs/ScanRegion.h>


namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Defines a set of parameters used to feed the renderer that specifies what and how to scan something.
 * 
 */
struct MicroscopeParameters {

    ScanRegion Region_; /**Defines the bounding box of the area we're scanning*/

    float VoxelResolution_um; /**Set the size of each voxel*/


};


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
