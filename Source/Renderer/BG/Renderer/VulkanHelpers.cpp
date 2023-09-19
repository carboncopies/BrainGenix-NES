#include <BG/Renderer/VulkanHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {


bool VulkanInit_CreateInstance(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // Configure Vulkan Builder Helper Instance
    _Logger->Log("Preparing To Create Vulkan Instance", 3);
    
    vkb::InstanceBuilder VulkanInstanceBuilder;
    VulkanInstanceBuilder.set_app_name("BrainGenix-NES Neuron Emulation System");
    VulkanInstanceBuilder.set_engine_name("BrainGenix-NES Rendering Engine");
    VulkanInstanceBuilder.require_api_version(1, 0, 0);


    _Logger->Log("Querying Information About Available Vulkan Layers", 3);
    static vkb::Result<vkb::SystemInfo> SystemInfoReturn = vkb::SystemInfo::get_system_info();
    if (!SystemInfoReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Get Vulkan System Information Data", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SystemInfoReturn.error().message();
        _Logger->Log(ErrorMessage, 10);
        
        return false;
    }

    vkb::SystemInfo SystemInfo = SystemInfoReturn.value();

    if (_RD->IsDebugging_) {
        _Logger->Log("Vulkan Validation Layers Have Been [REQUESTED]", 2);

        if (SystemInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
            _Logger->Log("Enabling Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 1);
            VulkanInstanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
        } else {
            _Logger->Log("Unable To Enable Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 7);
        }

        if (SystemInfo.validation_layers_available) {
            _Logger->Log("Enabling Vulkan Validation Layers", 1);
            VulkanInstanceBuilder.enable_validation_layers();
        } else {
            _Logger->Log("Unable To Enable Vulkan Validation Layers, As They Are Not Available (Check If They Have Been Installed?)", 8);
        }
    } else {
        _Logger->Log("Vulkan Validation Layers Have Been [DISABLED]", 2);
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
    _Logger->Log("FIXME: Add Our Own Vulkan Debug Callback System [Renderer]", 8);
    VulkanInstanceBuilder.use_default_debug_messenger(); // FIXME, SEE ABOVE FIXME NOTICE!
    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    // Was that enough FIXMEs?
    // I think we need more.



    // Now, Create The Vulkan Instance
    _Logger->Log("Building Vulkan Instance", 4);
    vkb::Result<vkb::Instance> VulkanInstanceBuilderReturn = VulkanInstanceBuilder.build();
    if (!VulkanInstanceBuilderReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Create Vulkan Instance (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanInstanceBuilderReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }

    _RD->VulkanInstance_ = VulkanInstanceBuilderReturn.value();

    return true;
}

bool VulkanInit_Optional_CreateWindow(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Debug Window Requested, Initializing SDL", 3);
    SDL_Init(SDL_INIT_VIDEO);

    _Logger->Log("Creating SDL Window", 3);
    SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
    _RD->Optional_SDLWindow_ = SDL_CreateWindow(
        "BrainGenix-NES Debugging Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        WindowFlags
    );
    if (_RD->Optional_SDLWindow_ == nullptr) {
        _Logger->Log("Failed to Create SDL Window (See Following Line For Error)", 10);
        std::string ErrorMessage = "SDL Reported Error: ";
        ErrorMessage += SDL_GetError();
        _Logger->Log(ErrorMessage, 10);
        return false;
    }

    _Logger->Log("Enabling Vulkan Extensions For Debug Window", 4);
    SDL_bool SurfaceCreateStatus = SDL_Vulkan_CreateSurface(_RD->Optional_SDLWindow_, _RD->VulkanInstance_.instance, &_RD->Optional_WindowSurface_);
    if (!SurfaceCreateStatus) {
        _Logger->Log("Failed to Create Vulkan Surface", 10);
        return false;
    }

    return true;
}

bool VulkanInit_CreateDevice(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);
   
    _Logger->Log("Selecting Physical Vulkan Capable Device From Available Devices", 3);
    vkb::PhysicalDeviceSelector VulkanPhysicalDeviceSelector(_RD->VulkanInstance_);
    
    /*
        A quick note about vk-bootstrap and headless/offscreen rendering.
        It seems that by default it does not support this using the PhysicalDeviceSelector, however, there is an option to enable it.
        This is done via the (somewhat confusingly named) 'defer_surface_initialization'. 
        Please see the following if statement, which is what indicates this is to be disabled (if the debug window is NOT enabled).
    */
    if (!_RD->IsWindowed_) {
        _Logger->Log("System Is Running Headless, No Presentation Surface Will Be Used", 1);
        VulkanPhysicalDeviceSelector.defer_surface_initialization();
    } else {
        _Logger->Log("System Is Windowed, Setting Presentation Surface", 1);
        VulkanPhysicalDeviceSelector.set_surface(_RD->Optional_WindowSurface_);
    }

    // Note that if we need any extra extensions/requirements, this is the place to tell the selector to use them.

    vkb::Result<vkb::PhysicalDevice> VulkanPhysicalDeviceSelectorReturn = VulkanPhysicalDeviceSelector.select();;
    if (!VulkanPhysicalDeviceSelectorReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Select Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanPhysicalDeviceSelectorReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    vkb::PhysicalDevice VulkanPhysicalDevice = VulkanPhysicalDeviceSelectorReturn.value();
    _Logger->Log(std::string("Selected Physical Device: ") + VulkanPhysicalDevice.name, 4);




    // Now, We Create The Logical Device (VkDevice) Instance On The Selected Physical Device
    _Logger->Log("Creating Logical Device On Selected Physical Device", 4);
    vkb::DeviceBuilder VulkanDeviceBuilder{VulkanPhysicalDevice};
    vkb::Result<vkb::Device> VulkanDeviceBuilderReturn = VulkanDeviceBuilder.build();
    if (!VulkanDeviceBuilderReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Create Logical Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanDeviceBuilderReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanDevice_ = VulkanDeviceBuilderReturn.value();

    return true;
}

bool VulkanInit_CreateQueues(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // Create Vulkan Queues
    _Logger->Log("Setting Up Vulkan Graphics Queue", 4);
    vkb::Result<VkQueue> VulkanGraphicsQueueReturn = _RD->VulkanDevice_.get_queue(vkb::QueueType::graphics);
    if (!VulkanGraphicsQueueReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Get Vulkan Graphics Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanGraphicsQueueReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanGraphicsQeueue_ = VulkanGraphicsQueueReturn.value();

    _Logger->Log("Setting Up Vulkan Transfer Queue", 4);
    vkb::Result<VkQueue> VulkanTransferQueueReturn = _RD->VulkanDevice_.get_queue(vkb::QueueType::transfer);
    if (!VulkanTransferQueueReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Get Vulkan Transfer Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanTransferQueueReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanTransferQeueue_ = VulkanTransferQueueReturn.value();

    return true;

}

bool VulkanInit_Optional_CreateSwapchain(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Setting Up Vulkan Swapchain For Windowed Rendering", 4);
    vkb::SwapchainBuilder VulkanSwapchainBuilder{_RD->VulkanDevice_};
    vkb::Result<vkb::Swapchain> SwapchainResult = VulkanSwapchainBuilder.build();
    if (!SwapchainResult) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Create Vulkan Swapchain (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SwapchainResult.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->Optional_Swapchain_ = SwapchainResult.value();


    return true;
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG