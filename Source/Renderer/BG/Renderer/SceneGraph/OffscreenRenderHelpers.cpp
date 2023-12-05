#include <BG/Renderer/SceneGraph/OffscreenRenderHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {

vsg::ref_ptr<vsg::ImageView> createColorImageView(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, VkFormat imageFormat, VkSampleCountFlagBits samples)
{
    auto colorImage = vsg::Image::create();
    colorImage->imageType = VK_IMAGE_TYPE_2D;
    colorImage->format = imageFormat;
    colorImage->extent = VkExtent3D{extent.width, extent.height, 1};
    colorImage->mipLevels = 1;
    colorImage->arrayLayers = 1;
    colorImage->samples = samples;
    colorImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImage->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    colorImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorImage->flags = 0;
    colorImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return vsg::createImageView(device, colorImage, VK_IMAGE_ASPECT_COLOR_BIT);
}

vsg::ref_ptr<vsg::ImageView> createDepthImageView(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, VkFormat depthFormat, VkSampleCountFlagBits samples)
{
    auto depthImage = vsg::Image::create();
    depthImage->imageType = VK_IMAGE_TYPE_2D;
    depthImage->extent = VkExtent3D{extent.width, extent.height, 1};
    depthImage->mipLevels = 1;
    depthImage->arrayLayers = 1;
    depthImage->samples = samples;
    depthImage->format = depthFormat;
    depthImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImage->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    depthImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImage->flags = 0;
    depthImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return vsg::createImageView(device, depthImage, vsg::computeAspectFlagsForFormat(depthFormat));
}

std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Image>> createColorCapture(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage, VkFormat sourceImageFormat)
{
    auto width = extent.width;
    auto height = extent.height;

    auto physicalDevice = device->getPhysicalDevice();

    VkFormat targetImageFormat = sourceImageFormat;

    //
    // 1) Check to see if Blit is supported.
    //
    VkFormatProperties srcFormatProperties;
    vkGetPhysicalDeviceFormatProperties(*(physicalDevice), sourceImageFormat, &srcFormatProperties);

    VkFormatProperties destFormatProperties;
    vkGetPhysicalDeviceFormatProperties(*(physicalDevice), VK_FORMAT_R8G8B8A8_UNORM, &destFormatProperties);

    bool supportsBlit = ((srcFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) != 0) &&
                        ((destFormatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) != 0);

    if (supportsBlit)
    {
        // we can automatically convert the image format when blit, so take advantage of it to ensure RGBA
        targetImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    }

    //std::cout<<"supportsBlit = "<<supportsBlit<<std::endl;

    //
    // 2) create image to write to
    //
    auto destinationImage = vsg::Image::create();
    destinationImage->imageType = VK_IMAGE_TYPE_2D;
    destinationImage->format = targetImageFormat;
    destinationImage->extent.width = width;
    destinationImage->extent.height = height;
    destinationImage->extent.depth = 1;
    destinationImage->arrayLayers = 1;
    destinationImage->mipLevels = 1;
    destinationImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    destinationImage->samples = VK_SAMPLE_COUNT_1_BIT;
    destinationImage->tiling = VK_IMAGE_TILING_LINEAR;
    destinationImage->usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    destinationImage->compile(device);

    auto deviceMemory = vsg::DeviceMemory::create(device, destinationImage->getMemoryRequirements(device->deviceID), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    destinationImage->bind(deviceMemory, 0);

    //
    // 3) create command buffer and submit to graphics queue
    //
    auto commands = vsg::Commands::create();

    // 3.a) transition destinationImage to transfer destination initialLayout
    auto transitionDestinationImageToDestinationLayoutBarrier = vsg::ImageMemoryBarrier::create(
        0,                                                             // srcAccessMask
        VK_ACCESS_TRANSFER_WRITE_BIT,                                  // dstAccessMask
        VK_IMAGE_LAYOUT_UNDEFINED,                                     // oldLayout
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                          // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                       // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                       // dstQueueFamilyIndex
        destinationImage,                                              // image
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1} // subresourceRange
    );

    // 3.b) transition swapChainImage from present to transfer source initialLayout
    auto transitionSourceImageToTransferSourceLayoutBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_MEMORY_READ_BIT,                                     // srcAccessMask
        VK_ACCESS_TRANSFER_READ_BIT,                                   // dstAccessMask
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                               // oldLayout
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                          // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                       // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                       // dstQueueFamilyIndex
        sourceImage,                                                   // image
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1} // subresourceRange
    );

    auto cmd_transitionForTransferBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_TRANSFER_BIT,                       // srcStageMask
        VK_PIPELINE_STAGE_TRANSFER_BIT,                       // dstStageMask
        0,                                                    // dependencyFlags
        transitionDestinationImageToDestinationLayoutBarrier, // barrier
        transitionSourceImageToTransferSourceLayoutBarrier    // barrier
    );

    commands->addChild(cmd_transitionForTransferBarrier);

    if (supportsBlit)
    {
        // 3.c.1) if blit using vkCmdBlitImage
        VkImageBlit region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.srcOffsets[0] = VkOffset3D{0, 0, 0};
        region.srcOffsets[1] = VkOffset3D{static_cast<int32_t>(width), static_cast<int32_t>(height), 1};
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.dstOffsets[0] = VkOffset3D{0, 0, 0};
        region.dstOffsets[1] = VkOffset3D{static_cast<int32_t>(width), static_cast<int32_t>(height), 1};

        auto blitImage = vsg::BlitImage::create();
        blitImage->srcImage = sourceImage;
        blitImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitImage->dstImage = destinationImage;
        blitImage->dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitImage->regions.push_back(region);
        blitImage->filter = VK_FILTER_NEAREST;

        commands->addChild(blitImage);
    }
    else
    {
        // 3.c.2) else use vkCmdCopyImage

        VkImageCopy region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.extent.width = width;
        region.extent.height = height;
        region.extent.depth = 1;

        auto copyImage = vsg::CopyImage::create();
        copyImage->srcImage = sourceImage;
        copyImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        copyImage->dstImage = destinationImage;
        copyImage->dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copyImage->regions.push_back(region);

        commands->addChild(copyImage);
    }

    // 3.d) transition destination image from transfer destination layout to general layout to enable mapping to image DeviceMemory
    auto transitionDestinationImageToMemoryReadBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_TRANSFER_WRITE_BIT,                                  // srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,                                     // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                          // oldLayout
        VK_IMAGE_LAYOUT_GENERAL,                                       // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                       // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                       // dstQueueFamilyIndex
        destinationImage,                                              // image
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1} // subresourceRange
    );

    // 3.e) transition swap chain image back to present
    auto transitionSourceImageBackToPresentBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_TRANSFER_READ_BIT,                                   // srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,                                     // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                          // oldLayout
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                               // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                       // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                       // dstQueueFamilyIndex
        sourceImage,                                                   // image
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1} // subresourceRange
    );

    auto cmd_transitionFromTransferBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_TRANSFER_BIT,                // srcStageMask
        VK_PIPELINE_STAGE_TRANSFER_BIT,                // dstStageMask
        0,                                             // dependencyFlags
        transitionDestinationImageToMemoryReadBarrier, // barrier
        transitionSourceImageBackToPresentBarrier      // barrier
    );

    commands->addChild(cmd_transitionFromTransferBarrier);

    return {commands, destinationImage};
}

std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Buffer>> createDepthCapture(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage, VkFormat sourceImageFormat)
{
    auto width = extent.width;
    auto height = extent.height;

    auto memoryRequirements = sourceImage->getMemoryRequirements(device->deviceID);

    // 1. create buffer to copy to.
    VkDeviceSize bufferSize = memoryRequirements.size;
    auto destinationBuffer = vsg::createBufferAndMemory(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VkImageAspectFlags imageAspectFlags = vsg::computeAspectFlagsForFormat(sourceImageFormat);

    // 2.a) transition depth image for reading
    auto commands = vsg::Commands::create();

    auto transitionSourceImageToTransferSourceLayoutBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, // srcAccessMask
        VK_ACCESS_TRANSFER_READ_BIT,                                                                // dstAccessMask
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,                                           // oldLayout
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                                                       // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                                                    // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                                                    // dstQueueFamilyIndex
        sourceImage,                                                                                // image
        VkImageSubresourceRange{imageAspectFlags, 0, 1, 0, 1}                                       // subresourceRange
    );

    auto transitionDestinationBufferToTransferWriteBarrier = vsg::BufferMemoryBarrier::create(
        VK_ACCESS_MEMORY_READ_BIT,    // srcAccessMask
        VK_ACCESS_TRANSFER_WRITE_BIT, // dstAccessMask
        VK_QUEUE_FAMILY_IGNORED,      // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,      // dstQueueFamilyIndex
        destinationBuffer,            // buffer
        0,                            // offset
        bufferSize                    // size
    );

    auto cmd_transitionSourceImageToTransferSourceLayoutBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
        VK_PIPELINE_STAGE_TRANSFER_BIT,                                                            // dstStageMask
        0,                                                                                         // dependencyFlags
        transitionSourceImageToTransferSourceLayoutBarrier,                                        // barrier
        transitionDestinationBufferToTransferWriteBarrier                                          // barrier
    );
    commands->addChild(cmd_transitionSourceImageToTransferSourceLayoutBarrier);

    // 2.b) copy image to buffer
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = width; // need to figure out actual row length from somewhere...
        region.bufferImageHeight = height;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = VkOffset3D{0, 0, 0};
        region.imageExtent = VkExtent3D{width, height, 1};

        auto copyImage = vsg::CopyImageToBuffer::create();
        copyImage->srcImage = sourceImage;
        copyImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        copyImage->dstBuffer = destinationBuffer;
        copyImage->regions.push_back(region);

        commands->addChild(copyImage);
    }

    // 2.c) transition depth image back for rendering
    auto transitionSourceImageBackToPresentBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_TRANSFER_READ_BIT,                                                                // srcAccessMask
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                                                       // oldLayout
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,                                           // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                                                    // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                                                    // dstQueueFamilyIndex
        sourceImage,                                                                                // image
        VkImageSubresourceRange{imageAspectFlags, 0, 1, 0, 1}                                       // subresourceRange
    );

    auto transitionDestinationBufferToMemoryReadBarrier = vsg::BufferMemoryBarrier::create(
        VK_ACCESS_TRANSFER_WRITE_BIT, // srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,    // dstAccessMask
        VK_QUEUE_FAMILY_IGNORED,      // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,      // dstQueueFamilyIndex
        destinationBuffer,            // buffer
        0,                            // offset
        bufferSize                    // size
    );

    auto cmd_transitionSourceImageBackToPresentBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_TRANSFER_BIT,                                                            // srcStageMask
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
        0,                                                                                         // dependencyFlags
        transitionSourceImageBackToPresentBarrier,                                                 // barrier
        transitionDestinationBufferToMemoryReadBarrier                                             // barrier
    );

    commands->addChild(cmd_transitionSourceImageBackToPresentBarrier);

    return {commands, destinationBuffer};
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG