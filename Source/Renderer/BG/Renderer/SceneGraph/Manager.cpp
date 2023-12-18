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

    // Support 3rd Party Format Through vsgXchange
    RenderData_->Options_->add(vsgXchange::all::create());


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

    Scene_->Camera_ = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(RenderData_->Window_->extent2D()));


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



bool Manager::SetupViewer() {


    if (!RenderData_->Headless_) {
        Windowed_CreateWindow();
    }

    SetupCamera();

    
    if (!RenderData_->Headless_) {
        Windowed_SetupEventHandler();
    }





    // // create a command graph to render the scene on the specified window
    auto commandGraph = vsg::createCommandGraphForView(RenderData_->Window_, Scene_->Camera_, Scene_->Group_);
    RenderData_->Viewer_->assignRecordAndSubmitTaskAndPresentation({commandGraph});

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