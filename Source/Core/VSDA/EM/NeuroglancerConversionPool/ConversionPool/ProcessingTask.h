
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-19
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
#include <atomic>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Common/Structs/ScanRegion.h>


namespace BG {
namespace NES {
namespace ConversionPool {


/**
 * @brief Structure that defines the work to be completed. This involves taking a pointer to the voxel array in question,
 * reading the voxels specified, and generating an image with some extra parameters. This is done with multiple threads, 
 * but since we're just reading from the voxelarray, no sync is needed for accessing this data. 
 * 
 */
struct ProcessingTask {

    bool IsSegmentation_; /**Set if the task is loading and writing segmap or jpeg image data*/
    std::string SourceFilePath_; /**Path where the image came from originally*/
    Simulator::VoxelIndexInfo IndexInfo_; /**Information about the image's voxel positions*/
    std::string OutputDirectoryBasePath_; /**Base of the path where the output is going*/
    int ReductionLevels_; /**Number of downsampling levels to do for this image */

    std::atomic_bool IsDone_ = false; /**Indicates if the given task is done or not*/


};

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
