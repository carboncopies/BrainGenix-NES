
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the VSDA Scan data area
    Additional Notes: None
    Date Created: 2023-12-02
    Author(s): Thomas Liao


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


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/SliceGenerator/Structs/VoxelArray.h>
#include <VSDA/SliceGenerator/Structs/ScanRegion.h>
#include <VSDA/SliceGenerator/Structs/MicroscopeParameters.h>


namespace BG {
namespace NES {
namespace Simulator {


enum VSDAState {
    VSDA_NOT_INITIALIZED=0,
    VSDA_INIT_BEGIN=1,
    VSDA_INIT_DONE=2,
    VSDA_RENDER_READY=3,
    VSDA_RENDER_DONE=4
};


/**
 * @brief Struct which is useful for storing the status of all information used by the VSDA subsystem.
 * This contains things like microscope position, VoxelArray, etc.
 * Is owned by all simulations.
 * 
 */
struct VSDAData {

    VSDAState State_ = VSDA_NOT_INITIALIZED; /**Enum indicating the current state of this instance of VSDAData, tells the processing system if we need to be rendered, etc.*/

    std::unique_ptr<VoxelArray> Array_; /**Pointer to the voxel array instance - stores the stuff being scanned*/ 
    MicroscopeParameters        Params_; /**Defines the microscope parameters for the current scan area*/
    ScanRegion                  Region_; /**Defines the scan region we're working on (for this microscope)*/


};

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
