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

    // Free All Allocations
    for (unsigned int i = 0; i < Allocations_.size(); i++) {
        vmaDestroyBuffer(Allocator_, Allocations_[i].Buffer_, Allocations_[i].Allocation_);
        free(Allocations_[i]);
    }

}

MemoryManager::CreateBuffer(size_t _Size, unsigned short _Usage) {

    VkBufferCreateInfo BufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    BufferInfo.size = 65536;
    BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo AllocInfo = {};
    AllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    Allocation* Alloc = new Allocation;
    Alloc->Buffer_ = VkBuffer();
    Alloc->Allocation_ = VmaAllocation();
    VkResult Status = vmaCreateBuffer(_RD->Allocator_, &BufferInfo, &AllocInfo, &Alloc->Buffer_, &Alloc->Allocation_, nullptr);

    if (Status != VK_SUCCESS) {
        free(Alloc);
        return false
    }
    Allocations_.append(Alloc);
    return true;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
