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
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vk_mem_alloc.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/RenderData.h>



namespace BG {
namespace NES {
namespace Renderer {

/**
 * @brief Vulkan Memory Management Helper. Will automatically help to allocate, deallocate and own all allocated memory for Vulkan. 
 * This class may not yet be thread-safe, so allocating from multiple threads may be dangerous (at least for now - we will fix this later).
 * 
 */
class MemoryManager {

private:

    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/

    VmaAllocator Allocator_; /**Vulkan memory allocation manager - simplifies buffer creation and other things like that*/
    

    


public:

    /**
     * @brief Constructor for the memory management system.
     * Note that this must be called after initializing vulkan to the point of selecting the physical device.
    */
    MemoryManager(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
     * Will also free all gpu memory not yet freed by the rest of the system.
     * Note that this should not be relied on to free memory generally - you should do it yourself.
    */
    ~MemoryManager();



};


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
