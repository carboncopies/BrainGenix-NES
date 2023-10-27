#include <BG/Renderer/VulkanHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {



bool VulkanRenderer_DrawFrame(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    // Wait For Vulkan Sync Fences
    vkWaitForFences(_RD->VulkanDevice_.device, 1, &_RD->VulkanInTransitFences_[_RD->CurrentFrame_], VK_TRUE, UINT64_MAX);


    // Get Next Image To Render To
    uint32_t ImageIndex = 0;
	VkResult AcquireNextImageStatus = vkAcquireNextImageKHR(_RD->VulkanDevice_.device, _RD->Optional_Swapchain_, UINT64_MAX, _RD->VulkanAvailableSemaphores_[_RD->CurrentFrame_], VK_NULL_HANDLE, &ImageIndex);

	if (AcquireNextImageStatus == VK_ERROR_OUT_OF_DATE_KHR) {
		return VulkanUtil_RecreateSwapchain(_Logger, _RD);
	} else if (AcquireNextImageStatus != VK_SUCCESS && AcquireNextImageStatus != VK_SUBOPTIMAL_KHR) {
        _Logger->Log("Failed To Acquire Vulkan Swapchain Image (See Following Line For Error String)", 10);
        _Logger->Log(std::to_string(AcquireNextImageStatus), 10);
		return false;
	}

	if (_RD->VulkanImageInTransit_[ImageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(_RD->VulkanDevice_.device, 1, &_RD->VulkanImageInTransit_[ImageIndex], VK_TRUE, UINT64_MAX);
	}
	_RD->VulkanImageInTransit_[ImageIndex] = _RD->VulkanInTransitFences_[_RD->CurrentFrame_];


    // Build Submission To Draw Next Frame
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore WaitSemaphores[] = {_RD->VulkanAvailableSemaphores_[_RD->CurrentFrame_]};
	VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = WaitSemaphores;
	SubmitInfo.pWaitDstStageMask = WaitStages;

	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &_RD->VulkanCommandBuffers_[ImageIndex];

	VkSemaphore SignalSemaphores[] = {_RD->VulkanFinishedSemaphores_[_RD->CurrentFrame_]};
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores;



    // Submit Draw Call
	vkResetFences(_RD->VulkanDevice_, 1, &_RD->VulkanInTransitFences_[_RD->CurrentFrame_]);

    VkResult SubmitStatus = vkQueueSubmit(_RD->VulkanGraphicsQeueue_, 1, &SubmitInfo, _RD->VulkanInTransitFences_[_RD->CurrentFrame_]);
	if (SubmitStatus != VK_SUCCESS) {
        _Logger->Log("Failed To Submit Draw Command Buffer", 10);
		return false;
	}


    // Build Request To Present New Image To Window
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;

	VkSwapchainKHR Swapchains[] = {_RD->Optional_Swapchain_};
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = Swapchains;
	PresentInfo.pImageIndices = &ImageIndex;


    // Submit Present Request
	VkResult QueuePresentStatus = vkQueuePresentKHR(_RD->VulkanPresentQueue_, &PresentInfo);
	if (QueuePresentStatus == VK_ERROR_OUT_OF_DATE_KHR || QueuePresentStatus == VK_SUBOPTIMAL_KHR) {
		return VulkanUtil_RecreateSwapchain(_Logger, _RD);
	} else if (QueuePresentStatus != VK_SUCCESS) {
        _Logger->Log("Failed To Present Swapchain Image", 10);
		return false;
	}


    // Tick The Current Frame Counter, Wrap Based On MaxFramesInTransit
	_RD->CurrentFrame_ = (_RD->CurrentFrame_ + 1) % _RD->MaxFramesInTransit_;


	return true;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG