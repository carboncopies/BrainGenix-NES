//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains various helper functions for managing vulkan. This includes initializing various aspects of the pipeline as well as other things.
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

#include <shaderc/shaderc.h>

#include <glm/glm.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_mem_alloc.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/Shader/ShaderHelpers.h>

#include <BG/Renderer/RenderData.h>
#include <BG/Renderer/VulkanMemoryAllocator.h>



namespace BG {
namespace NES {
namespace Renderer {







/**
 * @brief Substep during Vulkan initialization. Creates the initial Vulkan instance.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateInstance(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);

/**
 * @brief Optional Substep during Vulkan initalization. Will create an SDL window, and enable associated vulkan extensions.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true
 * @return false 
 */
bool VulkanInit_Optional_CreateWindow(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);

/**
 * @brief Substep during Vulkan initalization. This function performs two critical steps during Vulkan init - firstly, it will enumerate physical devices and select a physical device. Then, it will create a logical device on the physical device selected.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateDevice(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Substep during Vulkan initialization. This function creates various queues in each of the required queue families, enabling us to talk to the gpu.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateQueues(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);

/**
 * @brief Optional substep during Vulkan initialization. This function optionally creates the swapchain for windowed rendering. 
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_Optional_CreateSwapchain(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Substep During Vulkan Initialization. This function creates Vulkan's renderpass.  
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateRenderPass(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);



/**
 * @brief Vulkan Init Helper, sets up the vulkan graphics pipeline, including the creation of any needed shaders.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _RD Pointer to system renderdata instance.
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateGraphicsPipeline(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Vulkan Init helper, gets info about the framebuffer in windowed mode, creates framebuffer in headless mode.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateFramebuffer(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Vulkan Init Helper. Creates the command pool for later creating command buffers so we can tell the GPU what to do.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateCommandPool(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);

/**
 * @brief Vulkan Initialization Helper. Creates the vertex buffer so we can transfer data in and out of the renderer.
 * This will likely need to be extended in the future to allow us to do this dynamically (for model streaming).
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to logging subsystem
 * @param _RD Pointer to render data struct
 * @param _MemManager Pointer to an initialized memory manager subsystem
 * @return true 
 * @return false 
*/
bool VulkanInit_CreateVertexBuffer(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD, MemoryManager* _MemManager);

/**
 * @brief Vulkan Init Helper. Creates the command buffers using the command pool, allowing us to tell the gpu what to do.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateCommandBuffers(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Vulkan Init helper, creates the synchronization objects needed for rendering.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanInit_CreateSyncObjects(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);



/**
 * @brief Vulkan util function. Recreates the swapchain to allow for window resizing.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanUtil_RecreateSwapchain(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);



/**
 * @brief Util Function to wait until the gpu is done doing whatever it is supposed to be. Used to make sure we're not in the middle of drawing something.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanUtil_WaitUntilGPUDone(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);


/**
 * @brief Vulkan Destructor Helper, Will Cleanup All Vulkan Objects Created. Should Be Called In Destructor Of the Manager.
 * Returns true on success, false on failure.
 * 
 * @param _Logger 
 * @param _RD 
 * @return true 
 * @return false 
 */
bool VulkanDeinit_DestroyAll(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG