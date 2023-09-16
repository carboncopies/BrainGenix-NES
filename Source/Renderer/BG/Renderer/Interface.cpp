#include <BG/Renderer/Interface.h>



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



Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger) {
    Logger_ = _Logger;

}



Interface::~Interface() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down NES Rendering Subsystem", 3);

    // Shutdown SDL (If Enabled)
    if (EnableDebugWindow_) {
        SDL_Quit();
    }

    // Cleanup Vulkan Objects
    Logger_->Log("Cleaning Up Optional Window Surface", 2);
    if (Optional_WindowSurface_) {
        vkb::destroy_surface(VulkanInstance_, Optional_WindowSurface_);
    }

    Logger_->Log("Cleaning Up Vulkan Logical Device", 2);
    if (VulkanDevice_) {
        vkb::destroy_device(VulkanDevice_);
    }

    Logger_->Log("Cleaning Up Vulkan Instance", 2);
    if (VulkanInstance_) {
        vkb::destroy_instance(VulkanInstance_);
    }

}


bool Interface::Initialize(bool _EnableDebugWindow, bool _EnableValidationLayers) {
    assert(Logger_ != nullptr);
    EnableDebugWindow_ = _EnableDebugWindow;
    Logger_->Log("Initializing NES Rendering Subsystem", 5);

    /*
    Vulkan Initialization Process Outline:

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

    // Configure Vulkan Builder Helper Instance
    Logger_->Log("Preparing To Create Vulkan Instance", 3);
    
    vkb::InstanceBuilder VulkanInstanceBuilder;
    VulkanInstanceBuilder.set_app_name("BrainGenix-NES Neuron Emulation System");
    VulkanInstanceBuilder.set_engine_name("BrainGenix-NES Rendering Engine");
    VulkanInstanceBuilder.require_api_version(1, 0, 0);


    Logger_->Log("Querying Information About Available Vulkan Layers", 3);
    static vkb::Result<vkb::SystemInfo> SystemInfoReturn = vkb::SystemInfo::get_system_info();
    if (!SystemInfoReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed To Get Vulkan System Information Data", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SystemInfoReturn.error().message();
        Logger_->Log(ErrorMessage, 10);
        
        return false;
    }
    return false;

    vkb::SystemInfo SystemInfo = SystemInfoReturn.value();

    if (_EnableValidationLayers) {
        Logger_->Log("Vulkan Validation Layers Have Been [REQUESTED]", 2);

        if (SystemInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
            Logger_->Log("Enabling Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 1);
            VulkanInstanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
        } else {
            Logger_->Log("Unable To Enable Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 7);
        }

        if (SystemInfo.validation_layers_available) {
            Logger_->Log("Enabling Vulkan Validation Layers", 1);
            VulkanInstanceBuilder.enable_validation_layers();
        } else {
            Logger_->Log("Unable To Enable Vulkan Validation Layers, As They Are Not Available (Check If They Have Been Installed?)", 8);
        }
    } else {
        Logger_->Log("Vulkan Validation Layers Have Been [DISABLED]", 2);
    }


    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    /*
    FUTURE TODO: ADD OUR OWN VULKAN DEBUG MESSAGE CALLBACK HANDLER 
    (ALLOWS US TO LOG VULKAN ERRORS USING THE BG LOGGER, NOT VIA std::cout/std::cerr)

    Something like this:

        instance_builder.set_debug_callback (
        [] (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void *pUserData) 
            -> VkBool32 {
                auto severity = vkb::to_string_message_severity (messageSeverity);
                auto type = vkb::to_string_message_type (messageType);
                printf ("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
                return VK_FALSE;
            }
        );
    
    */
    Logger_->Log("FIXME: Add Our Own Vulkan Debug Callback System [Renderer]", 8);
    VulkanInstanceBuilder.use_default_debug_messenger(); // FIXME, SEE ABOVE FIXME NOTICE!
    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    // Was that enough FIXMEs?
    // I think we need more.



    // Now, Create The Vulkan Instance
    Logger_->Log("Building Vulkan Instance", 4);
    vkb::Result<vkb::Instance> VulkanInstanceBuilderReturn = VulkanInstanceBuilder.build();
    if (!VulkanInstanceBuilderReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed To Create Vulkan Instance (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanInstanceBuilderReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }

    VulkanInstance_ = VulkanInstanceBuilderReturn.value();


    // Now, We Optionally Create The Debug Window
    if (_EnableDebugWindow) {

        Logger_->Log("Debug Window Requested, Initializing SDL", 3);
        SDL_Init(SDL_INIT_VIDEO);

        Logger_->Log("Creating SDL Window", 3);
	    SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
        Optional_SDLWindow_ = SDL_CreateWindow(
            "BrainGenix-NES Debugging Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            WindowFlags
        );
        if (Optional_SDLWindow_ == nullptr) {
            Logger_->Log("Failed to Create SDL Window (See Following Line For Error)", 10);
            std::string ErrorMessage = "SDL Reported Error: ";
            ErrorMessage += SDL_GetError();
            Logger_->Log(ErrorMessage, 10);
            return false;
        }

        Logger_->Log("Enabling Vulkan Extensions For Debug Window", 4);
        SDL_bool SurfaceCreateStatus = SDL_Vulkan_CreateSurface(Optional_SDLWindow_, VulkanInstance_.instance, &Optional_WindowSurface_);
        if (!SurfaceCreateStatus) {
            Logger_->Log("Failed to Create Vulkan Surface", 10);
            return false;
        }
    }



    // Next, We Select the Physical Device To Use
    Logger_->Log("Selecting Physical Vulkan Capable Device From Available Devices", 3);
    vkb::PhysicalDeviceSelector VulkanPhysicalDeviceSelector(VulkanInstance_);
    
    /*
        A quick note about vk-bootstrap and headless/offscreen rendering.
        It seems that by default it does not support this using the PhysicalDeviceSelector, however, there is an option to enable it.
        This is done via the (somewhat confusingly named) 'defer_surface_initialization'. 
        Please see the following if statement, which is what indicates this is to be disabled (if the debug window is NOT enabled).
    */
    if (!_EnableDebugWindow) {
        Logger_->Log("System Is Running Headless, No Presentation Surface Will Be Used", 1);
        VulkanPhysicalDeviceSelector.defer_surface_initialization();
    } else {
        Logger_->Log("System Is Windowed, Setting Presentation Surface", 1);
        VulkanPhysicalDeviceSelector.set_surface(Optional_WindowSurface_);
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
    VulkanDevice_ = VulkanDeviceBuilderReturn.value();




    // Create Vulkan Queues
    Logger_->Log("Setting Up Vulkan Graphics Queue", 4);
    vkb::Result<VkQueue> VulkanGraphicsQueueReturn = VulkanDevice_.get_queue(vkb::QueueType::graphics);
    if (!VulkanGraphicsQueueReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed to Get Vulkan Graphics Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanGraphicsQueueReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    VulkanGraphicsQeueue_ = VulkanGraphicsQueueReturn.value();

    Logger_->Log("Setting Up Vulkan Transfer Queue", 4);
    vkb::Result<VkQueue> VulkanTransferQueueReturn = VulkanDevice_.get_queue(vkb::QueueType::transfer);
    if (!VulkanTransferQueueReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed to Get Vulkan Transfer Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanTransferQueueReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }
    VulkanTransferQeueue_ = VulkanTransferQueueReturn.value();



    return true;
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG