//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the sets of functions used to render each frame.
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


// Third-Party Libraries (BG convention: use <> instead of "")
#include <VkBootstrap.h>

#include <SDL.h>
#include <SDL_vulkan.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/VulkanShaders.h>
#include <BG/Renderer/RenderData.h>



namespace BG {
namespace NES {
namespace Renderer {





/**
 * @brief Vulkan Renderer Helper. Main Renderer Function, Draws The Frame.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanRenderer_DrawFrame(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG