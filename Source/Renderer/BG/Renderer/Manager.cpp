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



    // Cleanup Windowed (Optional) Vulkan Objects
    if (RenderData_.Optional_Swapchain_) {
        Logger_->Log("Cleaning Up Optional Swapchain Surface", 2);
        vkb::destroy_swapchain(RenderData_.Optional_Swapchain_);
    }

    if (RenderData_.Optional_WindowSurface_) {
        Logger_->Log("Cleaning Up Optional Window Surface", 2);
        vkb::destroy_surface(RenderData_.VulkanInstance_, RenderData_.Optional_WindowSurface_);
    }

    // Shutdown SDL (If Enabled)
    if (RenderData_.IsWindowed_) {
        SDL_Quit();
    }


    // Cleanup Required Vulkan Objects
    Logger_->Log("Cleaning Up Vulkan Logical Device", 2);
    if (RenderData_.VulkanDevice_) {
        vkb::destroy_device(RenderData_.VulkanDevice_);
    }

    Logger_->Log("Cleaning Up Vulkan Instance", 2);
    if (RenderData_.VulkanInstance_) {
        vkb::destroy_instance(RenderData_.VulkanInstance_);
    }
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


    // Setup RenderData Struct With Configuration Options
    Logger_->Log("Initializing Vulkan Handle Struct", 2);
    RenderData_.IsReady_     = false;
    RenderData_.IsDebugging_ = _IsDebugging;
    RenderData_.IsWindowed_  = _IsWindowed;

    // Now, hand this off to the vulkan initialization helper
    if (!VulkanInit_CreateInstance(Logger_, &RenderData_)) {
        // Note that error messages are made by the init func, so we can just return false here
        return false;
    }

    


    // Now, We Optionally Create The Renderer Window
    if (RenderData_.IsWindowed_) {

        Logger_->Log("Debug Window Requested, Initializing SDL", 3);
        SDL_Init(SDL_INIT_VIDEO);

        Logger_->Log("Creating SDL Window", 3);
	    SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
        RenderData_.Optional_SDLWindow_ = SDL_CreateWindow(
            "BrainGenix-NES Debugging Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            WindowFlags
        );
        if (RenderData_.Optional_SDLWindow_ == nullptr) {
            Logger_->Log("Failed to Create SDL Window (See Following Line For Error)", 10);
            std::string ErrorMessage = "SDL Reported Error: ";
            ErrorMessage += SDL_GetError();
            Logger_->Log(ErrorMessage, 10);
            return false;
        }

        Logger_->Log("Enabling Vulkan Extensions For Debug Window", 4);
        SDL_bool SurfaceCreateStatus = SDL_Vulkan_CreateSurface(RenderData_.Optional_SDLWindow_, RenderData_.VulkanInstance_.instance, &RenderData_.Optional_WindowSurface_);
        if (!SurfaceCreateStatus) {
            Logger_->Log("Failed to Create Vulkan Surface", 10);
            return false;
        }
    }



    // Next, We Select the Physical Device To Use
    Logger_->Log("Selecting Physical Vulkan Capable Device From Available Devices", 3);
    vkb::PhysicalDeviceSelector VulkanPhysicalDeviceSelector(RenderData_.VulkanInstance_);
    
    /*
        A quick note about vk-bootstrap and headless/offscreen rendering.
        It seems that by default it does not support this using the PhysicalDeviceSelector, however, there is an option to enable it.
        This is done via the (somewhat confusingly named) 'defer_surface_initialization'. 
        Please see the following if statement, which is what indicates this is to be disabled (if the debug window is NOT enabled).
    */
    if (!RenderData_.IsWindowed_) {
        Logger_->Log("System Is Running Headless, No Presentation Surface Will Be Used", 1);
        VulkanPhysicalDeviceSelector.defer_surface_initialization();
    } else {
        Logger_->Log("System Is Windowed, Setting Presentation Surface", 1);
        VulkanPhysicalDeviceSelector.set_surface(RenderData_.Optional_WindowSurface_);
    }

    // Note that if we need any extra extensions/requirements, this is the place to tell the selector to use them.

    vkb::Result<vkb::PhysicalDevice> VulkanPhysicalDeviceSelectorReturn = VulkanPhysicalDeviceSelector.select();;
    if (!VulkanPhysicalDeviceSelectorReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed To Select Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanPhysicalDeviceSelectorReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    vkb::PhysicalDevice VulkanPhysicalDevice = VulkanPhysicalDeviceSelectorReturn.value();
    Logger_->Log(std::string("Selected Physical Device: ") + VulkanPhysicalDevice.name, 4);




    // Now, We Create The Logical Device (VkDevice) Instance On The Selected Physical Device
    Logger_->Log("Creating Logical Device On Selected Physical Device", 4);
    vkb::DeviceBuilder VulkanDeviceBuilder{VulkanPhysicalDevice};
    vkb::Result<vkb::Device> VulkanDeviceBuilderReturn = VulkanDeviceBuilder.build();
    if (!VulkanDeviceBuilderReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed To Create Logical Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanDeviceBuilderReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    RenderData_.VulkanDevice_ = VulkanDeviceBuilderReturn.value();




    // Create Vulkan Queues
    Logger_->Log("Setting Up Vulkan Graphics Queue", 4);
    vkb::Result<VkQueue> VulkanGraphicsQueueReturn = RenderData_.VulkanDevice_.get_queue(vkb::QueueType::graphics);
    if (!VulkanGraphicsQueueReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed to Get Vulkan Graphics Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanGraphicsQueueReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    RenderData_.VulkanGraphicsQeueue_ = VulkanGraphicsQueueReturn.value();

    Logger_->Log("Setting Up Vulkan Transfer Queue", 4);
    vkb::Result<VkQueue> VulkanTransferQueueReturn = RenderData_.VulkanDevice_.get_queue(vkb::QueueType::transfer);
    if (!VulkanTransferQueueReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed to Get Vulkan Transfer Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanTransferQueueReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    RenderData_.VulkanTransferQeueue_ = VulkanTransferQueueReturn.value();


    // Create Swapchain Optionally If Rendering To A Window
    if (RenderData_.IsWindowed_) {
        Logger_->Log("Setting Up Vulkan Swapchain For Windowed Rendering", 4);
        vkb::SwapchainBuilder VulkanSwapchainBuilder{RenderData_.VulkanDevice_};
        vkb::Result<vkb::Swapchain> SwapchainResult = VulkanSwapchainBuilder.build();
        if (!SwapchainResult) {
            Logger_->Log("Error During Renderer Initialization", 10);
            Logger_->Log("Failed to Create Vulkan Swapchain (See Following Line For Error)", 10);
            std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SwapchainResult.error().message();
            Logger_->Log(ErrorMessage, 10);

            return false;
        }
        RenderData_.Optional_Swapchain_ = SwapchainResult.value();

    }


    return true;
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