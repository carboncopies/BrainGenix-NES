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




    return true;
}


bool Manager::Headless_SetupDevice() {

    // create instance
    vsg::Names instanceExtensions;
    vsg::Names requestedLayers;
    bool debugLayer = true;
    bool apiDumpLayer = true;
    if (debugLayer || apiDumpLayer) {
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
        if (apiDumpLayer) requestedLayers.push_back("VK_LAYER_LUNARG_api_dump");
    }

    vsg::Names validatedNames = vsg::validateInstancelayerNames(requestedLayers);
    uint32_t VulkanVersion = VK_API_VERSION_1_2;

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

    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

    VkExtent2D Extent = RenderData_->Extent_;

    RenderData_->colorImageView = createColorImageView(RenderData_->Headless_Device_, Extent, imageFormat, VK_SAMPLE_COUNT_1_BIT);
    RenderData_->depthImageView = createDepthImageView(RenderData_->Headless_Device_, Extent, depthFormat, VK_SAMPLE_COUNT_1_BIT);
    if (samples == VK_SAMPLE_COUNT_1_BIT) {
        auto renderPass = vsg::createRenderPass(RenderData_->Headless_Device_, imageFormat, depthFormat, true);
        RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{RenderData_->colorImageView, RenderData_->depthImageView} , Extent.width, Extent.height, 1);
    } else {
        auto msaa_colorImageView = createColorImageView(RenderData_->Headless_Device_, Extent, imageFormat, samples);
        auto msaa_depthImageView = createDepthImageView(RenderData_->Headless_Device_, Extent, depthFormat, samples);

        auto renderPass = vsg::createMultisampledRenderPass(RenderData_->Headless_Device_, imageFormat, depthFormat, samples, true);
        RenderData_->framebuffer = vsg::Framebuffer::create(renderPass, vsg::ImageViews{msaa_colorImageView, RenderData_->colorImageView, msaa_depthImageView, RenderData_->depthImageView}, Extent.width, Extent.height, 1);
    }

    // create support for copying the color buffer
    std::tie(RenderData_->colorBufferCapture, RenderData_->copiedColorBuffer) = createColorCapture(RenderData_->Headless_Device_, Extent, RenderData_->colorImageView->image, imageFormat);
    std::tie(RenderData_->depthBufferCapture, RenderData_->copiedDepthBuffer) = createDepthCapture(RenderData_->Headless_Device_, Extent, RenderData_->depthImageView->image, depthFormat);
    

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
    RenderData_->Viewer_->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    return true;

}


bool Manager::Headless_SetupCommandGraph() {
    auto renderGraph = vsg::RenderGraph::create();

    renderGraph->framebuffer = RenderData_->framebuffer;
    renderGraph->renderArea.offset = {0, 0};
    renderGraph->renderArea.extent = RenderData_->Extent_;
    renderGraph->setClearValues({{1.0f, 1.0f, 0.0f, 0.0f}}, VkClearDepthStencilValue{0.0f, 0});

    auto view = vsg::View::create(Scene_->Camera_, Scene_->Group_);

    // vsg::CommandGraphs commandGraphs;
    // if (useExecuteCommands)
    // {
    //     auto secondaryCommandGraph = vsg::SecondaryCommandGraph::create(device, queueFamily);
    //     secondaryCommandGraph->addChild(view);
    //     secondaryCommandGraph->framebuffer = framebuffer;
    //     commandGraphs.push_back(secondaryCommandGraph);

    //     auto executeCommands = vsg::ExecuteCommands::create();
    //     executeCommands->connect(secondaryCommandGraph);

    //     renderGraph->contents = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
    //     renderGraph->addChild(executeCommands);
    // }
    // else
    renderGraph->addChild(view);

    auto commandGraph = vsg::CommandGraph::create(RenderData_->Headless_Device_, RenderData_->QueueFamily_);
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

    bool Status = RenderData_->Viewer_->advanceToNextFrame();
    if (!Status) {
        return false;
    }

    // pass any events into EventHandlers assigned to the Viewer
    RenderData_->Viewer_->handleEvents();
    RenderData_->Viewer_->update();
    RenderData_->Viewer_->recordAndSubmit();
    RenderData_->Viewer_->present();

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