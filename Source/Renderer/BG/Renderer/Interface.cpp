#include <BG/Renderer/Interface.h>



namespace BG {
namespace NES {
namespace Renderer {



Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger) {
    Logger_ = _Logger;

}



Interface::~Interface() {

}


bool Interface::Initialize(bool _EnableDebugWindow, bool _EnableValidationLayers) {
    assert(Logger_ != nullptr);
    EnableDebugWindow_ = _EnableDebugWindow;
    Logger_->Log("Initializing NES Rendering Subsystem", 5);

    /*
    Vulkan Initialization Process Outline:

        1. Initialize Vulkan
        2. [if enabled] Request Vulkan Validation Layers (for debugging purposes)
        3. 

    */

    // Create Vulkan Instance
    Logger_->Log("Creating Vulkan Instance", 3);
    
    vkb::InstanceBuilder VulkanInstanceBuilder;
    VulkanInstanceBuilder.set_app_name("BrainGenix-NES Neuron Emulation System");
    VulkanInstanceBuilder.set_engine_name("BrainGenix-NES Rendering Engine");
    VulkanInstanceBuilder.require_api_version(1, 0, 0);

    vkb::Result<vkb::Instance> VulkanInstanceBuilderReturn = VulkanInstanceBuilder.build();
    if (!VulkanInstanceBuilderReturn) {
        Logger_->Log("Error During Renderer Initialization", 10);
        Logger_->Log("Failed To Create Vulkan Instance (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanInstanceBuilderReturn.error().message();
        Logger_->Log(ErrorMessage, 10);

        return false;
    }

    // Setup Vulkan Instance
    vkb::Instance VulkanInstance = VulkanInstanceBuilderReturn.value();
    if (_EnableValidationLayers) {
        Logger_->Log("Vulkan Validation Layers Have Been [ENABLED]", 2);
        VulkanInstanceBuilder.request_validation_layers();
    } else {
        Logger_->Log("Vulkan Validation Layers Have Been [DISABLED]", 2);
    }


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




    return true;
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG