#define VMA_IMPLEMENTATION

#include <BG/Renderer/VulkanMemoryAllocator.h>


namespace BG {
namespace NES {
namespace Renderer {


MemoryManager::MemoryManager(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr && "_Logger is a null pointer! Do not pass a null pointer.");
    assert(_RD != nullptr && "_RD is null, cannot pass null pointer.");

    // Copy Over Logger Pointer
    Logger_ = _Logger;


    // Setup VMA Allocator Instance
    Logger_->Log("Setting Up Vulkan Memory Management Subsystem", 3);
    VmaVulkanFunctions VulkanFunctions = {};
    VulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    VulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    
    VmaAllocatorCreateInfo AllocatorCreateInfo = {};
    AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    AllocatorCreateInfo.physicalDevice = _RD->VulkanDevice_.physical_device;
    AllocatorCreateInfo.device = _RD->VulkanDevice_.device;
    AllocatorCreateInfo.instance = _RD->VulkanInstance_.instance;
    // AllocatorCreateInfo.pVulkanFunctions = &VulkanFunctions;
    
    vmaCreateAllocator(&AllocatorCreateInfo, &Allocator_);

}

MemoryManager::~MemoryManager() {

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
