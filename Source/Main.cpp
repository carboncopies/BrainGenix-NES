//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

/*
    Description: This is the main file of the system.
    Additional Notes: None
    Date Created: 2021-11-01
*/


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_LogSystemInfo.h>
#include <ERS_HardwareInformation.h>
#include <ERS_ArgumentParser.h>

#include <ERS_STRUCT_SystemUtils.h>




int main(int NumArguments, char** ArguemntValues) {

    // Initialize System Vars
    std::unique_ptr<ERS_STRUCT_SystemUtils> SystemUtils = std::make_unique<ERS_STRUCT_SystemUtils>();
    SystemUtils->SystemShouldRun_ = std::make_unique<bool>(true);

    // Load Local System Configuration File
    SystemUtils->LocalSystemConfiguration_ = std::make_unique<YAML::Node>(YAML::LoadFile("Config.yaml"));

    // Instantiate Logging Subsystem
    SystemUtils->Logger_ = std::make_unique<ERS_LoggingSystem>(*SystemUtils->LocalSystemConfiguration_.get());
    SystemUtils->Logger_->Log("Initialized Logging System", 5);

    // Handle Command Line Arguments
    BrainGenix::ERS::Module::ArgumentParser ArgumentParser = BrainGenix::NES::Module::ArgumentParser(SystemUtils->Logger_.get());
    ArgumentParser.ParseArguments(NumArguments, ArguemntValues);
    SystemUtils->ArgumentString_ = ArgumentParser.GetArgumentString();
    SystemUtils->Arguments_ = ArgumentParser.GetArgumentPairs();
    

    // Setup Framerate Manager
    SystemUtils->Logger_->Log("Initializing Framerate Manager Subsystem", 5);
    SystemUtils->FramerateManager_ = std::make_unique<ERS_CLASS_FramerateManager>();
    SystemUtils->Logger_->Log("Initialized Framerate Manager", 4);

    // Copy Config Params
    try {
        SystemUtils->Logger_->Log("Reading Configuration File For 'IsLinux'", 2);
        SystemUtils->IsLinux_ = (*SystemUtils->LocalSystemConfiguration_.get())["IsLinux"].as<bool>();
    } catch(YAML::BadSubscript&) {
        SystemUtils->Logger_->Log("Error Reading Configuration File For 'IsLinux' Boolean Value, Defaulting To False", 9);
    }

    // Startup IO Subsystem And Other Related Systems
    SystemUtils->ERS_IOSubsystem_ = std::make_unique<ERS_InputOutputSubsystem>(
        SystemUtils->Logger_.get(),
        *SystemUtils->LocalSystemConfiguration_.get(),
        SystemUtils->Arguments_
    );
    SystemUtils->ERS_HardwareInformation_ = std::make_unique<ERS_HardwareInformation>(
        SystemUtils->Logger_.get(),
        *SystemUtils->LocalSystemConfiguration_.get()
    );

    SystemUtils->ERS_CLASS_PythonInterpreterIntegration_ = std::make_unique<ERS_CLASS_PythonInterpreterIntegration>(
        SystemUtils->Logger_.get()
    );


    ERS_LogSystemInfo(SystemUtils->Logger_.get());


    // Initialize Times
    float DeltaTime = 0.0f;
    float LastFrame = 0.0f;

    // Enter Main Loop
    while (*SystemUtils->SystemShouldRun_) {

        // Calculate Frametime
        float CurrentTime = glfwGetTime();
        DeltaTime = CurrentTime - LastFrame;
        LastFrame = CurrentTime;

        // Calculate Last FrameTime
        SystemUtils->FramerateManager_->StartNewFrame();



        // Update Joysticks
        HIDUtils->ControllerInputManager->UpdateControllers();


        // Update Renderers
        sERSRendererManager.UpdateLoop(DeltaTime);



        // End Frame
        SystemUtils->FramerateManager_->DelayUntilNextFrame();

    }


    // Test Clear Project
    ProjectUtils->SceneManager_->Scenes_.erase(ProjectUtils->SceneManager_->Scenes_.begin(), ProjectUtils->SceneManager_->Scenes_.end());


    // Program Exit (Destructors Automatically Called)
    return 0;

}

