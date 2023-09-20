#include <BG/Renderer/VulkanHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {



bool VulkanRenderer_DrawFrame(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // 

}



// bool VulkanDeinit_DestroyAll(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
//     assert(_Logger != nullptr);
//     assert(_RD != nullptr);

//     // Cleanup Windowed (Optional) Vulkan Objects
//     if (_RD->Optional_Swapchain_) {
//         _Logger->Log("Cleaning Up Optional Swapchain Surface", 2);
//         _RD->Optional_Swapchain_.destroy_image_views(_RD->VulkanSwapchainImageViews_);
//         vkb::destroy_swapchain(_RD->Optional_Swapchain_);
//     }

//     if (_RD->Optional_WindowSurface_) {
//         _Logger->Log("Cleaning Up Optional Window Surface", 2);
//         vkb::destroy_surface(_RD->VulkanInstance_, _RD->Optional_WindowSurface_);
//     }


//     // Shutdown SDL (If Enabled)
//     if (_RD->IsWindowed_) {
//         SDL_Quit();
//     }


//     // Cleanup Required Vulkan Objects
//     _Logger->Log("Cleaning Up Vulkan Sync Objects", 2);
//     for (unsigned int i = 0; i < _RD->MaxFramesInTransit_; i++) {
//         vkDestroySemaphore(_RD->VulkanDevice_.device, _RD->VulkanFinishedSemaphores_[i], nullptr);
//         vkDestroySemaphore(_RD->VulkanDevice_.device, _RD->VulkanAvailableSemaphores_[i], nullptr);
//         vkDestroyFence(_RD->VulkanDevice_.device, _RD->VulkanInTransitFences_[i], nullptr);
//     }


//     _Logger->Log("Cleaning Up Vulkan Command Pool", 2);
//     if (_RD->VulkanCommandPool_) {
//         vkDestroyCommandPool(_RD->VulkanDevice_.device, _RD->VulkanCommandPool_, nullptr);
//     }

//     _Logger->Log("Cleaning Up Vulkan Pipeline", 2);
//     if (_RD->VulkanGraphicsPipeline_) {
//         vkDestroyPipeline(_RD->VulkanDevice_.device, _RD->VulkanGraphicsPipeline_, nullptr);
//     }

//     _Logger->Log("Cleaning Up Vulkan Pipeline Layout", 2);
//     if (_RD->VulkanPipelineLayout_) {
//         vkDestroyPipelineLayout(_RD->VulkanDevice_.device, _RD->VulkanPipelineLayout_, nullptr);
//     }

//     _Logger->Log("Cleaning Up Vulkan Renderpass", 2);
//     if (_RD->VulkanRenderPass_) {
//         vkDestroyRenderPass(_RD->VulkanDevice_.device, _RD->VulkanRenderPass_, nullptr);
//     }

//     _Logger->Log("Cleaning Up Vulkan Framebuffers", 2);
//     for (unsigned int i = 0; i < _RD->VulkanFramebuffers_.size(); i++) {
//         vkDestroyFramebuffer(_RD->VulkanDevice_.device, _RD->VulkanFramebuffers_[i], nullptr);
//     }

//     _Logger->Log("Cleaning Up Vulkan Logical Device", 2);
//     if (_RD->VulkanDevice_) {
//         vkb::destroy_device(_RD->VulkanDevice_);
//     }

//     _Logger->Log("Cleaning Up Vulkan Instance", 2);
//     if (_RD->VulkanInstance_) {
//         vkb::destroy_instance(_RD->VulkanInstance_);
//     }

//     return true;

// }




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG