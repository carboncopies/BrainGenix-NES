
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the VSDA Scan data area
    Additional Notes: None
    Date Created: 2024-02-12
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

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>
#include <VSDA/Ca/VoxelSubsystem/Structs/CaMicroscopeParameters.h>
#include <VSDA/Common/Structs/ScanRegion.h>

#include <VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h>
#include <Simulator/Structs/CalciumImaging.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


enum CaState {
    CA_NOT_INITIALIZED=0,
    CA_INIT_BEGIN=1,
    CA_INIT_DONE=2,
    CA_RENDER_REQUESTED=3,
    CA_RENDER_IN_PROGRESS=4,
    CA_RENDER_DONE=5
};


/**
 * @brief Struct which is useful for storing the Result of all information used by the VSDA subsystem.
 * This contains things like microscope position, VoxelArray, etc.
 * Is owned by all simulations.
 * 
 */
struct CalciumImagingData {

    CaState                     State_ = CA_NOT_INITIALIZED; /**Enum indicating the current state of this instance of VSDAData, tells the processing system if we need to be rendered, etc.*/

    std::unique_ptr<VoxelArray> Array_;              /**Pointer to the voxel array instance - stores the stuff being scanned*/ 
    CaMicroscopeParameters      Params_;             /**Defines the microscope parameters for the current scan area*/
    std::vector<Simulator::ScanRegion> Regions_;            /**Defines the list of scan region we're working on (for this microscope) Use ActiveRegionID to get the current region*/
    int                         ActiveRegionID_ =-1; /**Defines the region's index that we're working on right now*/
   
    // Result Info For API To Query
    int                         TotalSlices_ = 0;        /**Defines the total number of slices to be rendered (is populated once the renderer begins)*/
    int                         CurrentSlice_ = 0;       /**Defines the current slice that is being rendered. (also is set by the renderer once initialization starts)*/
    int                         TotalSliceImages_ = 0;   /**Defines the total number of images for this slice*/
    int                         CurrentSliceImage_ = 0;  /**Defines the current image being worked on for this slice*/
    int                         TotalRegions_ = 0;       /**Defines the total number of subregions that must be rendered*/
    int                         CurrentRegion_ = 0;      /**Defines the current region being rendered by the system right now.*/



    std::vector<std::vector<std::string>> RenderedImagePaths_; /**List of paths for each region to be populated as we render all the images for this simulation into a stack*/
    std::vector<std::unique_ptr<ProcessingTask>> Tasks_; /**List of tasks that have been created for this render operation, we check that they're all done before finishing our render operation*/

    std::vector<std::vector<float>>* CalciumConcentrationByIndex_; /**Pointer to vector containing all the calcium concentrations*/
    float CalciumConcentrationTimestep_ms; /**Timestep of each index in the calcium concentrations*/


    Simulator::Tools::CalciumImaging CaImaging;

};



}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
