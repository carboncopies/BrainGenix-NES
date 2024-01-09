//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-12-04

    Code here has been taken from the vsgExamples git repository.
*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <assert.h>


// Third-Party Libraries (BG convention: use <> instead of "")

#include <vsg/all.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/Math/MathHelpers.h>


namespace BG {
namespace NES {
namespace Renderer {

/**
 * @brief Helper function which creates a Vulkan ColorImageView object.
 * 
 * @param device 
 * @param extent 
 * @param imageFormat 
 * @param samples 
 * @return vsg::ref_ptr<vsg::ImageView> 
 */
vsg::ref_ptr<vsg::ImageView> createColorImageView(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, VkFormat imageFormat, VkSampleCountFlagBits samples);

/**
 * @brief Helper function which creates a DepthImageView object.
 * 
 * @param device 
 * @param extent 
 * @param depthFormat 
 * @param samples 
 * @return vsg::ref_ptr<vsg::ImageView> 
 */
vsg::ref_ptr<vsg::ImageView> createDepthImageView(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, VkFormat depthFormat, VkSampleCountFlagBits samples);

/**
 * @brief Helper function which creates a ColorCapture object.
 * 
 * @param device 
 * @param extent 
 * @param sourceImage 
 * @param sourceImageFormat 
 * @return std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Image>> 
 */
std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Image>> createColorCapture(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage, VkFormat sourceImageFormat);

/**
 * @brief Helper function which creates a DepthCapture object.
 * 
 * @param device 
 * @param extent 
 * @param sourceImage 
 * @param sourceImageFormat 
 * @return std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Buffer>> 
 */
std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Buffer>> createDepthCapture(vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage, VkFormat sourceImageFormat);


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG