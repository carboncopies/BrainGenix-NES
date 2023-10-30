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
    RenderData_->WindowTraits_ = vsg::WindowTraits::create(); // TODO: Make this support headless as a config option
    RenderData_->WindowTraits_->windowTitle = "BrainGenix-NES";

    // Initialize Default Values, Overwrite With Manually Specified Ones
    vsg::CommandLine Arguments(&_NumArgs, _ArgValues);
    RenderData_->WindowTraits_->debugLayer = Arguments.read({"--debug","-d"});
    RenderData_->WindowTraits_->apiDumpLayer = Arguments.read({"--api","-a"});
    if (Arguments.read({"--fullscreen", "--fs"})) {
        RenderData_->WindowTraits_->fullscreen = true;
    }
    if (Arguments.read({"--window", "-w"}, RenderData_->WindowTraits_->width, RenderData_->WindowTraits_->height)) {
        RenderData_->WindowTraits_->fullscreen = false;
    }
    // auto HorizonMountainHeight = Arguments.value(0.0, "--hmh");
    Arguments.read("--screen", RenderData_->WindowTraits_->screenNum);
    Arguments.read("--display", RenderData_->WindowTraits_->display);

    if (Arguments.errors()) {
        Logger_->Log("Error Parsing Arguments For Renderer", 10);
        return false;
    }

    // Support 3rd Party Format Through vsgXchange
    RenderData_->Options_->add(vsgXchange::all::create());


    return true;
}


bool Manager::SetupScene() {

    Logger_->Log("Setting Up Scene Structure", 1);
    Scene_->Group_ = vsg::Group::create();

    // read any vsg files from command line arguments
    // for (int i=1; i<_NumArgs; ++i)
    // {
    //     vsg::Path filename = arguments[i];
    //     auto loaded_scene = vsg::read_cast<vsg::Node>(filename, options);
    //     if (loaded_scene)
    //     {
    //         scene->addChild(loaded_scene);
    //         arguments.remove(i, 1);
    //         --i;
    //     }
    // }


    // -----------------------------------------------------------------------------------------------------------------------------------------
    // add this later when we have the api call to setup scene
    // -----------------------------------------------------------------------------------------------------------------------------------------

    // scene->addChild(vsg::Builder::createBox());

    // if (scene->children.empty())
    // {
    //     std::cout<<"No scene loaded, please specify valid 3d model(s) on command line."<<std::endl;
    //     return;
    // }

    return true;

}


bool Manager::SetupViewer() {



    // create the viewer and assign window(s) to it
    RenderData_->Viewer_ = vsg::Viewer::create();

    RenderData_->Window_ = vsg::Window::create(RenderData_->WindowTraits_);
    if (!RenderData_->Window_) {
        Logger_->Log("Failed To Create Window", 10);
        return false;
    }

    RenderData_->Viewer_->addWindow(RenderData_->Window_);

    // compute the bounds of the scene graph to help position the camera
    vsg::ComputeBounds computeBounds;
    Scene_->Group_->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min+computeBounds.bounds.max)*0.5;
    double radius = vsg::length(computeBounds.bounds.max-computeBounds.bounds.min)*0.6;
    double nearFarRatio = 0.0001;

    // set up the camera
    auto lookAt = vsg::LookAt::create(centre+vsg::dvec3(0.0, -radius*3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

    vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
    // if (vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(Scene_->Group_->getObject<vsg::EllipsoidModel>("EllipsoidModel")); ellipsoidModel)
    // {
    //     // EllipsoidPerspective is useful for whole earth databases where per frame management of the camera's near & far values is optimized
    //     // to the current view relative to an ellipsoid model of the earth so that when near to the earth the near and far planes are pulled in close to the eye
    //     // and when far away from the earth's surface the far plane is pushed out to ensure that it encompasses the horizon line, accounting for mountains over the horizon.
    //     perspective = vsg::EllipsoidPerspective::create(lookAt, ellipsoidModel, 30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio, horizonMountainHeight);
    // }
    // else
    // {
    perspective = vsg::Perspective::create(30.0, static_cast<double>(RenderData_->Window_->extent2D().width) / static_cast<double>(RenderData_->Window_->extent2D().height), nearFarRatio*radius, radius * 4.5);
    // }

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(RenderData_->Window_->extent2D()));

    // add close handler to respond to pressing the window's close window button and to pressing escape
    RenderData_->Viewer_->addEventHandler(vsg::CloseHandler::create(RenderData_->Viewer_));

    // add a trackball event handler to control the camera view using the mouse
    RenderData_->Viewer_->addEventHandler(vsg::Trackball::create(camera));

    // create a command graph to render the scene on the specified window
    auto commandGraph = vsg::createCommandGraphForView(RenderData_->Window_, camera, Scene_->Group_);
    RenderData_->Viewer_->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    // compile all the Vulkan objects and transfer data required to render the scene
    RenderData_->Viewer_->compile();

    return true;
}




bool Manager::DrawFrame() {

    // rendering main loop
    while (RenderData_->Viewer_->advanceToNextFrame())
    {
        // pass any events into EventHandlers assigned to the Viewer
        RenderData_->Viewer_->handleEvents();
        RenderData_->Viewer_->update();
        RenderData_->Viewer_->recordAndSubmit();
        RenderData_->Viewer_->present();
    }

    return false;
}


vsg::ref_ptr<vsg::Group> Manager::GetScene() {
    return Scene_->Group_;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG