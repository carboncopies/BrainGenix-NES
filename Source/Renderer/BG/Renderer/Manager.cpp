#include <BG/Renderer/Manager.h>



/**
 * MEMORY LEAK NOTICE:
 * 
 * There are false positives in this code! 
 * It's not actually leaking memory, valgrind just repots memory leaks due to dl_open calls.
 * To *actually* check for memory leaks, please use a null icd driver (see this for more info):
 *     https://github.com/KhronosGroup/Vulkan-Loader/issues/256
 * 
 * THIS IS A KNOWN BUG IN VALGRIND!
*/

namespace BG {
namespace NES {
namespace Renderer {



Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

}

Manager::~Manager() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down Rendering Subsystem", 3);

    VulkanUtil_WaitUntilGPUDone(Logger_, &RenderData_);

    VulkanDeinit_DestroyAll(Logger_, &RenderData_);
    
}


bool Manager::Initialize(bool _IsWindowed, bool _IsDebugging) {
    assert(Logger_ != nullptr);
    Logger_->Log("Initializing NES Rendering Subsystem", 5);

    /*
    Vulkan Initialization Process Outline:

        0. Setup RenderData Struct Instance (owns all vulkan handles and other render config data)
        1. Initialize Vulkan
        2. Tell Vulkan some info about our application (such as name, engine, etc.)
        3. [if enabled] Request Vulkan Validation Layers (for debugging purposes)
        4. Build vulkan instance
        5. [if enabled] create window for debugging purposes
        6. Find a suitable physical device (GPU card or something like that)
        7. Create a logical device on the physical device (let us use the physical device)
        8. Setup queues (graphics, transfer, compute, etc.) these let us talk to the gpu and tell it to do things
        9. [if enabled] Setup Swapchain (what is shown onscreen)
    */


    // Setup RenderData Struct With Configuration Options, Create Instance
    Logger_->Log("Initializing Vulkan Handle Struct", 2);
    RenderData_.IsReady_     = false;
    RenderData_.IsDebugging_ = _IsDebugging;
    RenderData_.IsWindowed_  = _IsWindowed;

    if (!VulkanInit_CreateInstance(Logger_, &RenderData_)) {
        // Note that error messages are made by the init func, so we can just return false here
        return false;
    }

    // Now, We Optionally Create The Renderer Window
    if (RenderData_.IsWindowed_) {
        if (!VulkanInit_Optional_CreateWindow(Logger_, &RenderData_)) {
            return false;
        }
    }

    // Next, We Select the Physical Device To Use, Create a logical device on that physical device
    if (!VulkanInit_CreateDevice(Logger_, &RenderData_)) {
        return false;
    }

    // Setup Memory Management Subsystem
    MemoryManager_ = std::make_unique<MemoryManager>(Logger_, &RenderData_);

    ShaderCompiler_ = std::make_unique<Internal::ShaderCompiler>(Logger_);


    // Create Swapchain Optionally If Rendering To A Window
    if (RenderData_.IsWindowed_) {
        if (!VulkanInit_Optional_CreateSwapchain(Logger_, &RenderData_)) {
            return false;
        }
    }

    // Now, We Need To Create Vulkan Queues
    if (!VulkanInit_CreateQueues(Logger_, &RenderData_)) {
        return false;
    }



    // Create Render Pass
    if (!VulkanInit_CreateRenderPass(Logger_, &RenderData_)) {
        return false;
    }

    // Create Graphics Pipeline
    if (!VulkanInit_CreateGraphicsPipeline(Logger_, &RenderData_, ShaderCompiler_.get())) {
        return false;
    }

    // Create Framebuffer (if windowed rendering enabled)
    if (!VulkanInit_CreateFramebuffer(Logger_, &RenderData_)) {
        return false;
    }

    // Create Command Pool
    if (!VulkanInit_CreateCommandPool(Logger_, &RenderData_)) {
        return false;
    }

    // Create Vertex Buffer
    if (!VulkanInit_CreateVertexBuffer(Logger_, &RenderData_, MemoryManager_.get())) {
        return false;
    }


    // Create Command Buffers
    if (!VulkanInit_CreateCommandBuffers(Logger_, &RenderData_)) {
        return false;
    }


    // Create Sync Objects
    if (!VulkanInit_CreateSyncObjects(Logger_, &RenderData_)) {
        return false;
    }


    // Phew, done. *wipes sweat from forehead*
    // That was a hell of a lot of work!
    // Why are we using Vulkan again?


    return true;
}

bool Manager::DrawFrame() {
    VulkanRenderer_DrawFrame(Logger_, &RenderData_);

    
    return false;
}


bool Manager::IsReady() {
    return RenderData_.IsReady_;
}

bool Manager::IsWindowed() {
    return RenderData_.IsWindowed_;
}

bool Manager::IsDebugging() {
    return RenderData_.IsDebugging_;
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG