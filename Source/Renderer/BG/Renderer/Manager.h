//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering Manager. It is responsible for the underlying initialization and ownership of the Vulkan subsystem.
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

#include <BG/Renderer/VulkanHelpers.h>
#include <BG/Renderer/Shader/ShaderHelpers.h>
#include <BG/Renderer/VulkanRenderer.h>
#include <BG/Renderer/RenderData.h>
#include <BG/Renderer/VulkanMemoryAllocator.h>



namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief The renderer manager is responsible for underlying Vulkan management (init, deinit, controling access, etc.)
 */
class Manager {

private: 

    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/

    RenderData RenderData_; /**Instance of Render Data Struct. Stores All Data About The Renderer (Vulkan Handles, etc)*/

    std::unique_ptr<MemoryManager> MemoryManager_; /**Instance of our own wrapper for a memory manager*/
   

public:

    /**
     * @brief Constructor for the rendering system.
    */
    Manager(BG::Common::Logger::LoggingSystem* _Logger);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
    */
    ~Manager();


    /**
     * @brief Initializes the Rendering interface. 
     * Use the _IsWindowed param for debugging by shwoing an SDL window containing the to-be-rendered content.
     * Note that use of this parameter is for debugging only and requires having either an X-Server or Wayland installed.
     * 
     * _IsDebugging as the name implies, toggles on and off debug/validation layers.
     * These layers serve to provide debug information for the application developers.
     * Only disable these after performing *extensive* testing, as bugs may not be noticed otherwise.
    */
    bool Initialize(bool _IsWindowed = false, bool _IsDebugging = true);

    /**
     * @brief Does what it sounds like, this draws a frame on the renderer.
     * Returns true on success, false on failure.
     * 
     * @return true 
     * @return false 
     */
    bool DrawFrame();


    /**
     * @brief Return true or false if the renderer is ready to be used to start drawing things.
     * Note that this should return false if Initialize has not been called yet.
     * This should return true if Initialize has been called and succeeded. If not, something went terribly wrong...
     * 
     * @return true 
     * @return false 
     */
    bool IsReady();

    /**
     * @brief Return true or false if the renderer has a window or not. Pretty self-explanitory.
     * Note that this will always return false if Initialize has not yet been called.
     * 
     * @return true 
     * @return false 
     */
    bool IsWindowed();

    /**
     * @brief Return true or false if the renderer is in debug mode or not.
     * Note that this will default to true if Initialize has not yet been called.
     * 
     * @return true 
     * @return false 
     */
    bool IsDebugging();



};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG