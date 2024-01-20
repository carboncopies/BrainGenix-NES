
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
#include <vector>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/VoxelSubsystem/Structs/ScanRegion.h>
#include <VSDA/VoxelSubsystem/Structs/MicroscopeParameters.h>

#include <VSDA/VoxelSubsystem/ImageProcessorPool/Image.h>



namespace BG {
namespace NES {
namespace Simulator {


enum VSDAState {
    VSDA_NOT_INITIALIZED=0,
    VSDA_INIT_BEGIN=1,
    VSDA_INIT_DONE=2,
    VSDA_RENDER_REQUESTED=3,
    VSDA_RENDER_IN_PROGRESS=4,
    VSDA_RENDER_DONE=5
};


/**
 * @brief Struct which is useful for storing the Result of all information used by the VSDA subsystem.
 * This contains things like microscope position, VoxelArray, etc.
 * Is owned by all simulations.
 * 
 */
struct VSDAData {

    VSDAState State_ = VSDA_NOT_INITIALIZED; /**Enum indicating the current state of this instance of VSDAData, tells the processing system if we need to be rendered, etc.*/

    std::unique_ptr<VoxelArray> Array_;              /**Pointer to the voxel array instance - stores the stuff being scanned*/ 
    MicroscopeParameters        Params_;             /**Defines the microscope parameters for the current scan area*/
    std::vector<ScanRegion>     Regions_;            /**Defines the list of scan region we're working on (for this microscope) Use ActiveRegionID to get the current region*/
    int                         ActiveRegionID_ =-1; /**Defines the region's index that we're working on right now*/
   
    // Result Info For API To Query
    int                         TotalSlices_ = 0;        /**Defines the total number of slices to be rendered (is populated once the renderer begins)*/
    int                         CurrentSlice_ = -1;      /**Defines the current slice that is being rendered. (also is set by the renderer once initialization starts)*/
    int                         TotalSliceImages_ = -1;   /**Defines the total number of images for this slice*/
    int                         CurrentSliceImage_ = -1;  /**Defines the current image being worked on for this slice*/

    std::vector<std::vector<std::string>> RenderedImagePaths_; /**List of paths for each region to be populated as we render all the images for this simulation into a stack*/
    std::vector<std::unique_ptr<Image>> Images_; /**List of images that have been created for this system*/



};

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
