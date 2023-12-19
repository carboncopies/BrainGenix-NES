#include <BG/Renderer/SceneGraph/Manager.h>



// TODO: Update Doxygen Docs!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



namespace BG {
namespace NES {
namespace Renderer {



Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

}

Manager::~Manager() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down Rendering Subsystem", 3);

}


bool Manager::Initialize(int _NumArgs, char** _ArgValues) {
    assert(Logger_ != nullptr);
    Logger_->Log("Initializing NES Rendering Subsystem", 5);

    // Iniitalize State Structs
    RenderData_ = std::make_unique<State::RenderData>();
    Scene_      = std::make_unique<State::Scene>();

    // Setup Renderer
    Logger_->Log("Setting Up Configuration Options", 1);
    RenderData_->Options_ = vsg::Options::create();
    RenderData_->WindowTraits_ = vsg::WindowTraits::create();
    RenderData_->WindowTraits_->windowTitle = "BrainGenix-NES";


    // Check if we're running windowed or not
    RenderData_->Headless_ = true;
    for (int i = 0; i < _NumArgs; i++) {
        if (std::string(_ArgValues[i]) == "--Windowed") {
            RenderData_->Headless_ = false;
        }
    }

    // Support 3rd Party Format Through vsgXchange
    RenderData_->Options_->add(vsgXchange::all::create());


    // Setup Device If Not Windowed
    if (RenderData_->Headless_) {
        Headless_SetupDevice();
    }
    RenderData_->Extent_ = VkExtent2D{(unsigned int)RenderData_->Width_, (unsigned int)RenderData_->Height_};


    vsg::ref_ptr<vsg::Node> teapot = vsg::read_cast<vsg::Node>("teapot.vsgt", vsg::Options::create());
    Scene_->Group_->addChild(teapot);


    return true;
}


bool Manager::Headless_SetupDevice() {

    // create instance
    vsg::Names instanceExtensions;
    vsg::Names requestedLayers;
    bool debugLayer = true;
    bool apiDumpLayer = false;
    if (debugLayer || apiDumpLayer) {
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
        if (apiDumpLayer) requestedLayers.push_back("VK_LAYER_LUNARG_api_dump");
    }

    vsg::Names validatedNames = vsg::validateInstancelayerNames(requestedLayers);
    uint32_t VulkanVersion = VK_API_VERSION_1_0;

    auto instance = vsg::Instance::create(instanceExtensions, validatedNames, VulkanVersion);
    auto [physicalDevice, queueFamily] = instance->getPhysicalDeviceAndQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    RenderData_->QueueFamily_ = queueFamily;
    if (!physicalDevice || queueFamily < 0) {
        std::cout << "Could not create PhysicalDevice" << std::endl;
        return false;
    }

    vsg::Names deviceExtensions;
    vsg::QueueSettings queueSettings{vsg::QueueSetting{queueFamily, {1.0}}};

    auto deviceFeatures = vsg::DeviceFeatures::create();
    deviceFeatures->get().samplerAnisotropy = VK_TRUE;
    // deviceFeatures->get().geometryShader = enableGeometryShader;

    RenderData_->Headless_Device_ = vsg::Device::create(physicalDevice, queueSettings, validatedNames, deviceExtensions, deviceFeatures);

    return true;

}

bool Manager::Headless_CreateRenderingBuffers() {



    VkExtent2D Extent = RenderData_->Extent_;

    RenderData_->colorImageView = createColorImageView(RenderData_->Headless_Device_, Extent, RenderData_->imageFormat, VK_SAMPLE_COUNT_1_BIT);
    std::cout<<"Created Color Image Veiw "<<RenderData_->imageFormat<<std::endl;
    RenderData_->depthImageView = createDepthImageView(RenderData_->Headless_Device_, Extent, RenderData_->depthFormat, VK_SAMPLE_COUNT_1_BIT);
    std::cout<<"Created depth Image Veiw "<<RenderData_->depthFormat<<std::endl;
    if (RenderData_->samples == VK_SAMPLE_COUNT_1_BIT) {
        auto renderPass = vsg::createRenderPass(RenderData_->Headless_Device_, RenderData_->imageFormat, RenderData_->depthFormat, true);
        RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{RenderData_->colorImageView, RenderData_->depthImageView} , Extent.width, Extent.height, 1);
    } else {
        auto msaa_colorImageView = createColorImageView(RenderData_->Headless_Device_, Extent, RenderData_->imageFormat, RenderData_->samples);
        auto msaa_depthImageView = createDepthImageView(RenderData_->Headless_Device_, Extent, RenderData_->depthFormat, RenderData_->samples);

        auto renderPass = vsg::createMultisampledRenderPass(RenderData_->Headless_Device_, RenderData_->imageFormat, RenderData_->depthFormat, RenderData_->samples, true);
        RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{msaa_colorImageView, RenderData_->colorImageView, msaa_depthImageView, RenderData_->depthImageView}, Extent.width, Extent.height, 1);
    }

    // create support for copying the color buffer
    std::tie(RenderData_->colorBufferCapture, RenderData_->copiedColorBuffer) = createColorCapture(RenderData_->Headless_Device_, Extent, RenderData_->colorImageView->image, RenderData_->imageFormat);
    std::tie(RenderData_->depthBufferCapture, RenderData_->copiedDepthBuffer) = createDepthCapture(RenderData_->Headless_Device_, Extent, RenderData_->depthImageView->image, RenderData_->depthFormat);
    

    return true;

}

bool Manager::Headless_UpdateRenderingBuffers() {


    std::cout << "Frame " << RenderData_->Viewer_->getFrameStamp()->frameCount << std::endl;
    int resizeCadence = 0;
    if (resizeCadence && (RenderData_->Viewer_->getFrameStamp()->frameCount>0) && ((RenderData_->Viewer_->getFrameStamp()->frameCount) % resizeCadence == 0))
    {
        RenderData_->Viewer_->deviceWaitIdle();

        RenderData_->Extent_.width /= 2;
        RenderData_->Extent_.height /= 2;

        if (RenderData_->Extent_.width < 1) RenderData_->Extent_.width = 1;
        if (RenderData_->Extent_.height < 1) RenderData_->Extent_.height = 1;

        std::cout << "Resized to " << RenderData_->Extent_.width << ", " << RenderData_->Extent_.height << std::endl;

        auto replace_child = [](vsg::Group* group, vsg::ref_ptr<vsg::Node> previous, vsg::ref_ptr<vsg::Node> replacement) {
            for (auto& child : group->children)
            {
                if (child == previous) child = replacement;
            }
        };

        auto previous_colorBufferCapture = RenderData_->colorBufferCapture;
        auto previous_depthBufferCapture = RenderData_->depthBufferCapture;

        
        RenderData_->colorImageView = createColorImageView(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->imageFormat, VK_SAMPLE_COUNT_1_BIT);
        RenderData_->depthImageView = createDepthImageView(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->depthFormat, VK_SAMPLE_COUNT_1_BIT);
        if (RenderData_->samples == VK_SAMPLE_COUNT_1_BIT)
        {
            auto renderPass = vsg::createRenderPass(RenderData_->Headless_Device_, RenderData_->imageFormat, RenderData_->depthFormat, true);
            RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{RenderData_->colorImageView, RenderData_->depthImageView} , RenderData_->Extent_.width, RenderData_->Extent_.height, 1);
        }
        else
        {
            auto msaa_colorImageView = createColorImageView(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->imageFormat, RenderData_->samples);
            auto msaa_depthImageView = createDepthImageView(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->depthFormat, RenderData_->samples);

            auto renderPass = vsg::createMultisampledRenderPass(RenderData_->Headless_Device_, RenderData_->imageFormat, RenderData_->depthFormat, RenderData_->samples, true);
            RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{msaa_colorImageView, RenderData_->colorImageView, msaa_depthImageView, RenderData_->depthImageView} , RenderData_->Extent_.width, RenderData_->Extent_.height, 1);
        }


        // create new copy subgraphs
        std::tie( RenderData_->colorBufferCapture,  RenderData_->copiedColorBuffer) = createColorCapture(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->colorImageView->image, RenderData_->imageFormat);
        std::tie( RenderData_->depthBufferCapture,  RenderData_->copiedDepthBuffer) = createDepthCapture(RenderData_->Headless_Device_, RenderData_->Extent_, RenderData_->depthImageView->image, RenderData_->depthFormat);

        replace_child(RenderData_->CommandGraph, previous_colorBufferCapture,  RenderData_->colorBufferCapture);
        replace_child(RenderData_->CommandGraph, previous_depthBufferCapture,  RenderData_->depthBufferCapture);
    



    }

    return true;

}

bool Manager::Headless_GetImage() {

    std::string colorFilename = "test.bmp";

    // wait for completion.
    uint64_t waitTimeout = 1999999999; // 2 seconds in nanoseconds.

    RenderData_->Viewer_->waitForFences(0, waitTimeout);

    if (RenderData_->copiedColorBuffer) {
        
        VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(*RenderData_->Headless_Device_, RenderData_->copiedColorBuffer->vk(RenderData_->Headless_Device_->deviceID), &subResource, &subResourceLayout);

        auto deviceMemory = RenderData_->copiedColorBuffer->getDeviceMemory(RenderData_->Headless_Device_->deviceID);

        size_t destRowWidth = RenderData_->Extent_.width * sizeof(vsg::ubvec4);
        vsg::ref_ptr<vsg::Data> imageData;
        if (destRowWidth == subResourceLayout.rowPitch) {
            std::cout<<"1\n";
            // Map the buffer memory and assign as a vec4Array2D that will automatically unmap itself on destruction.
            imageData = vsg::MappedData<vsg::ubvec4Array2D>::create(deviceMemory, subResourceLayout.offset, 0, vsg::Data::Properties{RenderData_->imageFormat}, RenderData_->Extent_.width, RenderData_->Extent_.height);
        } else {
            std::cout<<"2\n";
            // Map the buffer memory and assign as a ubyteArray that will automatically unmap itself on destruction.
            // A ubyteArray is used as the graphics buffer memory is not contiguous like vsg::Array2D, so map to a flat buffer first then copy to Array2D.
            auto mappedData = vsg::MappedData<vsg::ubyteArray>::create(deviceMemory, subResourceLayout.offset, 0, vsg::Data::Properties{RenderData_->imageFormat}, subResourceLayout.rowPitch*RenderData_->Extent_.height);
            imageData = vsg::ubvec4Array2D::create(RenderData_->Extent_.width, RenderData_->Extent_.height, vsg::Data::Properties{RenderData_->imageFormat});
            for (uint32_t row = 0; row < RenderData_->Extent_.height; ++row) {
                std::memcpy(imageData->dataPointer(row*RenderData_->Extent_.width), mappedData->dataPointer(row * subResourceLayout.rowPitch), destRowWidth);
            }
        }

        // colorFilename = "screenshot.vsgb";
        // std::cout<<imageData<<std::endl;
        // std::cout<<imageData.get()->dataPointer()<<std::endl;
        // std::ofstream ofs ("/tmp/example.png", std::ofstream::out);
        // bool status = vsg::write(imageData, colorFilename);
        vsgXchange::all Instance;
        bool status = Instance.write(imageData, colorFilename);
        std::cout<<"Wrote File "<<colorFilename<<std::endl;;
        if (!status) {
            std::cout<<"Error writing file!\n";
        }

    }

    return true;

}


bool Manager::SetupScene() {

    Logger_->Log("Setting Up Scene Structure", 1);
    Scene_->Group_ = vsg::Group::create();

    return true;

}


bool Manager::SetupCamera() {

    // Setup Camera
    vsg::dvec3 CameraPosition = vsg::dvec3(0.0, 0.0, -3.0); // Where the camera is located
    vsg::dvec3 CameraTarget = vsg::dvec3(0.0, 0.0, 0.0); // Where the camera is looking towards
    auto lookAt = CreateLookAtMatrix(CameraPosition, CameraTarget, vsg::dvec3(0.0, 1.0, 0.0));

    double FOV = 80.f; // FOV In degrees
    int Width, Height;
    if (RenderData_->Headless_) {
        Width = RenderData_->Width_;
        Height = RenderData_->Height_;
    } else {
        Width = RenderData_->Window_->extent2D().width;
        Height = RenderData_->Window_->extent2D().height;
    }
    double AspectRatio = static_cast<double>(Width / (double)Height); // Just the aspect ratio of the window
    vsg::ref_ptr<vsg::ProjectionMatrix> perspective = CreatePerspectiveMatrix(FOV, AspectRatio, 0.1, 100.0);

    Scene_->Camera_ = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(RenderData_->Extent_));


    return true;

}

bool Manager::Windowed_CreateWindow() {

    // create the viewer and assign window(s) to it
    RenderData_->Viewer_ = vsg::Viewer::create();
    RenderData_->Window_ = vsg::Window::create(RenderData_->WindowTraits_);
    if (!RenderData_->Window_) {
        Logger_->Log("Failed To Create Window", 10);
        return false;
    }
    RenderData_->Viewer_->addWindow(RenderData_->Window_);
    RenderData_->Extent_ = VkExtent2D{RenderData_->Window_->extent2D().width, RenderData_->Window_->extent2D().height};

    return true;

}

bool Manager::Windowed_SetupEventHandler() {

    // Split into event handler function (separate?) -------------------------------------------------------------------------------------
    // add close handler to respond to pressing the window's close window button and to pressing escape
    RenderData_->Viewer_->addEventHandler(vsg::CloseHandler::create(RenderData_->Viewer_));
    // add a trackball event handler to control the camera view using the mouse
    RenderData_->Viewer_->addEventHandler(vsg::Trackball::create(Scene_->Camera_));

    return true;

}

bool Manager::Windowed_SetupCommandGraph() {

    // create a command graph to render the scene on the specified window
    auto commandGraph = vsg::createCommandGraphForView(RenderData_->Window_, Scene_->Camera_, Scene_->Group_);
    RenderData_->CommandGraph = commandGraph;
    RenderData_->Viewer_->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    return true;

}

bool Manager::Headless_SetupCommandGraph() {


    // bool useExecuteCommands = true;



    auto renderGraph = vsg::RenderGraph::create();

    renderGraph->framebuffer = RenderData_->framebuffer;
    renderGraph->renderArea.offset = {0, 0};
    renderGraph->renderArea.extent = RenderData_->Extent_;
    renderGraph->setClearValues({{1.0f, 1.0f, 0.0f, 0.0f}}, VkClearDepthStencilValue{0.0f, 0});

    auto view = vsg::View::create(Scene_->Camera_, Scene_->Group_);

    vsg::CommandGraphs commandGraphs;
    
    // if (useExecuteCommands)
    // {
    //     auto secondaryCommandGraph = vsg::SecondaryCommandGraph::create(RenderData_->Headless_Device_, RenderData_->QueueFamily_);
    //     secondaryCommandGraph->addChild(view);
    //     secondaryCommandGraph->framebuffer = RenderData_->framebuffer;
    //     commandGraphs.push_back(secondaryCommandGraph);

    //     auto executeCommands = vsg::ExecuteCommands::create();
    //     executeCommands->connect(secondaryCommandGraph);

    //     renderGraph->contents = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
    //     renderGraph->addChild(executeCommands);
    // }
    // else
    renderGraph->addChild(view);

    auto commandGraph = vsg::CommandGraph::create(RenderData_->Headless_Device_, RenderData_->QueueFamily_);
    RenderData_->CommandGraph = commandGraph;
    commandGraph->addChild(renderGraph);
    RenderData_->CommandGraphs_.push_back(commandGraph);
    if (RenderData_->colorBufferCapture) {
        commandGraph->addChild(RenderData_->colorBufferCapture);
    }
    if (RenderData_->depthBufferCapture) {
        commandGraph->addChild(RenderData_->depthBufferCapture);
    }

    return true;
}

bool Manager::SetupViewer() {


    if (!RenderData_->Headless_) {
        Windowed_CreateWindow();
    }

    SetupCamera();

    
    if (!RenderData_->Headless_) {
        Windowed_SetupEventHandler();
    } else {
        Headless_CreateRenderingBuffers();
    }



    if (!RenderData_->Headless_) {
        Windowed_SetupCommandGraph();
    } else {
        Headless_SetupCommandGraph();
    }


    if (RenderData_->Headless_) {
        RenderData_->Viewer_ = vsg::Viewer::create();

        // if (pathFilename)
        // {
        //     auto animationPath = vsg::read_cast<vsg::AnimationPath>(pathFilename, options);
        //     if (!animationPath)
        //     {
        //         std::cout<<"Warning: unable to read animation path : "<<pathFilename<<std::endl;
        //         return 1;
        //     }
        //     viewer->addEventHandler(vsg::AnimationPathHandler::create(camera, animationPath, viewer->start_point()));
        // }

        RenderData_->Viewer_->assignRecordAndSubmitTaskAndPresentation(RenderData_->CommandGraphs_);

        // viewer->compile();
    }


    // Compile Scene
    CompileScene();


    return true;
}



bool Manager::CompileScene() {
    RenderData_->Viewer_->compile();
    return true;
}

bool Manager::ClearScene() {

    LockScene();
    WaitUntilGPUDone();
        Scene_->Group_->children.clear();
    UnlockScene();

    return true;

}

bool Manager::DrawFrame() {


    // rendering main loop

    LockScene(); // Control Thread Safety

    
    // Called every frame
    bool Status = RenderData_->Viewer_->advanceToNextFrame();
    if (!Status) {
        return false;
    }


    // if (RenderData_->Headless_) { // only happens when we need to resize the buffer (which is never for now)
    //     Headless_UpdateRenderingBuffers();
    // }


    // pass any events into EventHandlers assigned to the Viewer
    RenderData_->Viewer_->handleEvents();
    RenderData_->Viewer_->update();
    RenderData_->Viewer_->recordAndSubmit();

    // If Headless, Save Image, Otherwise Present To Window
    if (RenderData_->Headless_) {
        Headless_GetImage();
    } else {
        RenderData_->Viewer_->present();
    }


    UnlockScene();

    std::this_thread::sleep_for(std::chrono::microseconds(750)); // delay so our mutex is easier to get by another thread if needed

    return true;
}


vsg::ref_ptr<vsg::Group> Manager::GetScene() {
    return Scene_->Group_;
}


void Manager::WaitUntilGPUDone() {
    RenderData_->Viewer_->deviceWaitIdle();
    return;
}

void Manager::LockScene() {
    SceneAccessLock_.lock();
}

void Manager::UnlockScene() {
    SceneAccessLock_.unlock();
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG