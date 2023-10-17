//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the RenderData struct, which contains all relevant variables (vulkan handles) for rendering.
    Additional Notes: None
    Date Created: 2023-09-15
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
#include <vector>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <VkBootstrap.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_mem_alloc.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Renderer/Allocation.h>


namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief This struct stores the renderer variables.
 * All Vulkan handles that need to persist are created and stored here for ease of access. 
 * Anything starting with the prefix Optional_ is only initialized if a window is enabled, and therefore should not be assumed to exist.
 * 
 * Other useful features of this struct:
 *  - bool IsReady_; -> Set to true only if the renderer has been initialized and is ready to start drawing stuff.
 *  - bool IsWindowed_; -> Only set to true when optional variables are initialized (ie: when the rendering system has a window).
 * 
*/
struct RenderData {

    // Allocator Info
    // VmaAllocator Allocator_; /**Vulkan memory allocation manager - simplifies buffer creation and other things like that*/
    Allocation* VertexBufferAllocation_; /**Pointer to vertex buffer allocation*/


    // Queues (How we talk to the GPU through Vulkan)
    VkQueue VulkanGraphicsQeueue_; /**Vulkan Graphics Queue Handle (Used for graphics operations, hence the name)*/
    VkQueue VulkanTransferQeueue_; /**Vulkan Transfer Queue Handle (Used to push/pull data to/from GPU VRAM and CPU RAM)*/
    VkQueue VulkanPresentQueue_;   /**Vulkan Present Queue Handle*/

    // Required Vulkan Handles (For Both Headless Mode And Windowed Mode)
    vkb::Instance VulkanInstance_; /**VulkanBoostrap Wrapper For Our Vulkan Instance*/
    vkb::Device VulkanDevice_; /**Vulkan Logical Device Wrapper*/

    VkFormat VulkanColorFormat_ = VK_FORMAT_R8G8B8A8_UNORM; /**Vulkan color format for render target (either vkImage in headless mode, or swapchain format in windowed mode). Note that in swapchain format, the default value is overwritten.*/

	std::vector<VkFramebuffer> VulkanFramebuffers_;


    std::vector<VkSemaphore> VulkanAvailableSemaphores_; /**Vulkan Sync objects not yet locked*/
    std::vector<VkSemaphore> VulkanFinishedSemaphores_; /**Vulkan Sync objects that have finished on the GPU*/
    std::vector<VkFence> VulkanInTransitFences_; 
    std::vector<VkFence> VulkanImageInTransit_; 
    unsigned int CurrentFrame_ = 0; /**Current frame being rendered.*/

    // maybe optional? Not sure yet.
	std::vector<VkImage> VulkanSwapchainImages_;
	std::vector<VkImageView> VulkanSwapchainImageViews_;

    // Optional Vulkan Handles (For Windowed Mode Only)
    VkSurfaceKHR Optional_WindowSurface_; /**Optional (If debug window is enabled) Window surface for presenting to a window*/
    SDL_Window* Optional_SDLWindow_ = nullptr; /**Optional (if debug window is enabled) Pointer to SDL Window object*/
    vkb::Swapchain Optional_Swapchain_; /**Optional (If debug window is enabled) Swapchain for rendering to window*/

    // Optional Vulkan Handles (For Headless Mode Only)
    

    // To Be sorted
    VkPipelineLayout VulkanPipelineLayout_;
    VkRenderPass VulkanRenderPass_;
    VkPipeline VulkanGraphicsPipeline_;
    
    VkCommandPool VulkanCommandPool_;
    std::vector<VkCommandBuffer> VulkanCommandBuffers_;



    // Status Indicator Variables
    bool IsReady_     = false; /**Indicates that the system is ready for rendering. True DOES NOT MEAN OPTIONAL VARIABLES ARE SETUP. YOU MUST CHECK IsWindowed_ FOR OPTIONAL VARS*/
    bool IsWindowed_  = false; /**Indicates if the renderer is running with a window or not. False means headless, true means windowed*/
    bool IsDebugging_ = false; /**Indicates if vulkan debug information is to be enabled or not.*/

    // Configuration Variables
    unsigned int Width_  = 800; /**Width in pixels that we should render at.*/
    unsigned int Height_ = 600; /**Height in pixels that we should render at.*/
    unsigned int MaxFramesInTransit_ = 3; /**Maxinum number of frames to be in transit to the screen at once.*/
    

};



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG