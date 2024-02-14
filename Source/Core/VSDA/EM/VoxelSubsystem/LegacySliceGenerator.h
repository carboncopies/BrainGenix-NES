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
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

#include <BG/Renderer/Interface.h>
#include <BG/Renderer/SceneGraph/Primitive/Cube.h>
#include <BG/Renderer/EncoderPool/Image.h>
#include <BG/Renderer/EncoderPool/EncoderPool.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {




/**
 * @brief Render the given slice from an array to the renderer's screen. Uses vulkan to do so.
 * 
 * @param _Logger 
 * @param _Renderer
 * @param _VSDAData 
 * @param _FileNameArray
 * @param _FilePrefix
 * @param _SliceNumber 
 * @return true 
 * @return false 
 */
bool VulkanRenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool);



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
