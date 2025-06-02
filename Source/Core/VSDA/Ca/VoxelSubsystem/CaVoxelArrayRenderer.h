//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface for the slice generator, which creates voxel representations of a given simulation.
    Additional Notes: None
    Date Created: 2024-02-12
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
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/Structs/CaMicroscopeParameters.h>
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>
#include <VSDA/Ca/VoxelSubsystem/Structs/CaData.h>

#include <VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>


#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


/**
 * @brief Render the given slice from an array to the renderer's screen.
 * 
 * @param _Logger Pointer to the logging system.
 * @param MaxImagesX Maximum number of images along the X-axis.
 * @param MaxImagesY Maximum number of images along the Y-axis.
 * @param _CaData Pointer to the calcium imaging data.
 * @param _Array Pointer to the voxel array to render.
 * @param _FilePrefix Prefix for the output file names.
 * @param SliceNumber The slice number to render.
 * @param _ImageProcessorPool Pointer to the image processor pool for handling image compression.
 * @param _OffsetX Offset along the X-axis (default is 0.0).
 * @param _OffsetY Offset along the Y-axis (default is 0.0).
 * @param _SliceOffset Offset for the slice index (default is 0).
 * @return std::vector<std::string> A vector of strings containing the paths to the rendered slice images.
 */
std::vector<std::string> CaRenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, int MaxImagesX, int MaxImagesY, CalciumImagingData* _CaData, VoxelArray* _Array, std::string _FilePrefix, int SliceNumber, ImageProcessorPool* _ImageProcessorPool, double _OffsetX=0., double _OffsetY=0., int _SliceOffset=0);




}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
