
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


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/SliceGenerator/Structs/VoxelArray.h>
#include <VSDA/SliceGenerator/Structs/ScanRegion.h>
#include <VSDA/SliceGenerator/Structs/MicroscopeParameters.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief Struct which is useful for storing the status of all information used by the VSDA subsystem.
 * This contains things like microscope position, VoxelArray, etc.
 * Is owned by all simulations.
 * 
 */
struct VSDAData {

    bool InUse_ = false; /**Denotes if this struct has been initialized and is ready to do stuff*/

    std::unique_ptr<VoxelArray> Array_; /**Pointer to the voxel array instance - stores the stuff being scanned*/ 
    MicroscopeParameters Params_; /**Defines the microscope parameters for the current scan area*/
    ScanRegion Region_; /**Defines the scan region we're working on (for this microscope)*/


}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
