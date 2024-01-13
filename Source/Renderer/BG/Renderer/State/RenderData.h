//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-10-26
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <assert.h>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/all.h>


// Internal Libraries (BG convention: use <> instead of "")




namespace BG {
namespace NES {
namespace Renderer {
namespace State {



/**
 * @brief Struct that contains the data which holds the state of the renderer.
 * This includes things like the window and associated high level vulkan abstractions.
 * It does NOT include things like the scene and as that is managed separately.
 * 
 * 
 */
struct RenderData {

    // Required Structs
    vsg::ref_ptr<vsg::Options>      Options_;      /**Vulkan Scene Graph Options Configuration*/
    vsg::ref_ptr<vsg::Viewer>       Viewer_;       /**Vulkan Scene Graph Viewer Object*/

    // Optional Data For Windowing
    vsg::ref_ptr<vsg::WindowTraits> WindowTraits_; /**If a window is enabled, this will be setup with windowing information.*/
    vsg::ref_ptr<vsg::Window>       Window_;       /**Only initialized if a window is created, holds the vsg window state*/


    std::atomic<int> NumFramesToRender_ = 1;       /**In headless mode, specify the number of frames to render before stopping, has no effect in windowed mode.*/



    int Width_ = 256;
    int Height_ = 256;

    bool Headless_ = false;


    vsg::ref_ptr<vsg::Device> Headless_Device_;



    vsg::ref_ptr<vsg::Framebuffer> framebuffer;
    vsg::ref_ptr<vsg::ImageView> colorImageView;
    vsg::ref_ptr<vsg::ImageView> depthImageView;
    vsg::ref_ptr<vsg::Commands> colorBufferCapture;
    vsg::ref_ptr<vsg::Image> copiedColorBuffer;
    vsg::ref_ptr<vsg::Commands> depthBufferCapture;
    vsg::ref_ptr<vsg::Buffer> copiedDepthBuffer;
    vsg::ref_ptr<vsg::RenderGraph> RenderGraph_;

    vsg::ref_ptr<vsg::CommandGraph> CommandGraph;

    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

    vsg::ref_ptr<vsg::Node> tmpscene;
    int framenumber = 0;


    vsg::CommandGraphs CommandGraphs_;

    VkExtent2D Extent_;

    int QueueFamily_;


};




}; // Close Namespace State
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG