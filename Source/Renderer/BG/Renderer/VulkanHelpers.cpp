#include <BG/Renderer/VulkanHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {


bool VulkanInit_CreateInstance(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
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


bool VulkanInit_CreateImageViews(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG