//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains various helper functions for managing shaders. This is intended to simplify the process of shader creation, usage, and initialization dramatically.
    Additional Notes: None
    Date Created: 2023-09-17
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
#include <iostream>
#include <assert.h>

#include <fstream> // temporary include, for testing precompiled shaders - use iosubsystem for this later


// Third-Party Libraries (BG convention: use <> instead of "")
#include <VkBootstrap.h>

// #include <SDL.h>
// #include <SDL_vulkan.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/RenderData.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {

/**
 * @brief Simple helper function for reading files.
 * 
 * @param _FileName 
 * @return std::vector<char> 
 */
std::vector<char> ReadFile(std::string _FileName);



/**
 * @brief Intermediate Vulkan Shader Helper, used to create a shader module from existing SPIR-V bytecode.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _RD Pointer to system renderdata instance.
 * @param _ShaderBytecode Pointer to vector of characters of the compiled SPIR-V bytecode.
 * @param _ShaderModule Pointer to the VkShaderModule to be populated
 * @return true 
 * @return false 
 */
bool Vulkan_CreateShaderModule(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD, const std::vector<char>* _ShaderBytecode, VkShaderModule* _ShaderModule);

}; // Close Namespace Internal



/**
 * @brief Vulkan Shader Helper, sets up the vulkan graphics pipeline, including the creation of any needed shaders.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _RD Pointer to system renderdata instance.
 * @return true 
 * @return false 
 */
bool Vulkan_CreateGraphicsPipeline(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG