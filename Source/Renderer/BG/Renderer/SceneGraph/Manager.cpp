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





bool Manager::DrawFrame() {

    

    return false;
}


bool Manager::IsReady() {
}

bool Manager::IsWindowed() {
}

bool Manager::IsDebugging() {
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG