//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface for the slice generator, which creates voxel representations of a given simulation.
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
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VSDAData.h>

#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>


#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Render the entire voxel array with the parameters defined in the various config structs.
 * Set the maximum number of simultaneous threads to the maxthreads argument.
 * 
 * @param _Logger 
 * @param _VSDAData 
 * @param _FilePrefix 
 * @param _ImageProcessorPool 
 * @param _MaxThreads 
 * @return std::vector<std::vector<std::string>> 
 */
std::vector<std::vector<std::string>> ThreadedRenderVoxelArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::string _FilePrefix, ImageProcessorPool* _ImageProcessorPool, int _MaxThreads=20);


/**
 * @brief Render the given slice from an array to the renderer's screen
 * 
 * @param _Logger 
 * @param _VSDAData 
 * @param _FilePrefix
 * @param _SliceNumber 
 * @return std::vector<std::string> 
 */
std::vector<std::string> RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, int MaxImagesX, int MaxImagesY, VSDAData* _VSDAData, VoxelArray* _Array, std::string _FilePrefix, int _SliceNumber, int _SliceThickness_vox, ImageProcessorPool* _ImageProcessorPool, double _OffsetX=0., double _OffsetY=0., int _SliceOffset=0);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
