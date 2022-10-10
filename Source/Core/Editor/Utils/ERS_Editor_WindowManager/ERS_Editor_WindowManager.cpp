//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_Editor_WindowManager.h>

/*
-- IMPORTANT NOTICE
-- PLEASE SEE README.MD IN WINDOWS DIRECTORY FOR INFO ABOUT ADDING NEW WINDOWS --
*/


ERS_CLASS_WindowManager::ERS_CLASS_WindowManager(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing WindowManager Subsystem", 4);

    
    Windows_ = std::make_unique<ERS_STRUCT_Windows>();

}

ERS_CLASS_WindowManager::~ERS_CLASS_WindowManager() {

    SystemUtils_->Logger_->Log("WindowManager Subsystem Destructor Called", 6);

}


// Setup Window Struct
void ERS_CLASS_WindowManager::GenerateWindowStruct(ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils, ERS_CLASS_VisualRenderer* VisualRenderer,
ERS_CLASS_ThemeManager* ThemeManager, ERS_CLASS_FontManager* FontManager, Cursors3D* Gizmo, ERS_CLASS_SceneManager* SceneManager) {

    // Setup Window System
    SystemUtils_->Logger_->Log("WindowManager Subsystem Setting Up Window Struct", 4);

    // Construct All Widnows -- PLEASE SEE README.MD IN WINDOWS DIRECTORY FOR INFO ABOUT ADDING NEW WINDOWS --
    Windows_->GUI_Window_About_                  = std::make_unique<GUI_Window_About>                  (SystemUtils_);
    Windows_->GUI_Window_AssetExplorer_          = std::make_unique<GUI_Window_AssetExplorer>          (SystemUtils_, ProjectUtils);
    Windows_->GUI_Window_ControllerSettings_     = std::make_unique<GUI_Window_ControllerSettings>     (SystemUtils_, HIDUtils, ProjectUtils);
    Windows_->GUI_Window_FrameLatencyGraph_      = std::make_unique<GUI_Window_FrameLatencyGraph>      (SystemUtils_);
    Windows_->GUI_Window_FramerateGraph_         = std::make_unique<GUI_Window_FramerateGraph>         ();
    Windows_->GUI_Window_FramerateCounter_       = std::make_unique<GUI_Window_FramerateCounter>       ();
    Windows_->GUI_Window_FramerateHistogram_     = std::make_unique<GUI_Window_FramerateHistogram>     ();
    Windows_->GUI_Window_FrameratePlot_          = std::make_unique<GUI_Window_FrameratePlot>          ();
    Windows_->GUI_Window_ImportProgressBar_      = std::make_unique<GUI_Window_ImportProgressBar>      (SystemUtils_);
    Windows_->GUI_Window_ObjectProperties_       = std::make_unique<GUI_Window_ObjectProperties>       (Gizmo,        SceneManager, ProjectUtils, VisualRenderer);
    Windows_->GUI_Window_ProjectSettings_        = std::make_unique<GUI_Window_ProjectSettings>        (ProjectUtils, SystemUtils_);
    Windows_->GUI_Window_RAMGraph_               = std::make_unique<GUI_Window_RAMGraph>               (SystemUtils_);
    Windows_->GUI_Window_RenderingSettings_      = std::make_unique<GUI_Window_RenderingSettings>      (SystemUtils_);
    Windows_->GUI_Window_EditorCameraSettings_   = std::make_unique<GUI_Window_EditorCameraSettings>   (SystemUtils_, VisualRenderer);
    Windows_->GUI_Window_SceneTree_              = std::make_unique<GUI_Window_SceneTree>              (SceneManager, SystemUtils_, ProjectUtils, Gizmo);
    Windows_->GUI_Window_ScriptEditor_           = std::make_unique<GUI_Window_ScriptEditor>           (SystemUtils_, ProjectUtils, VisualRenderer);
    Windows_->GUI_Window_ShaderEditor_           = std::make_unique<GUI_Window_ShaderEditor>           (SystemUtils_, ProjectUtils, VisualRenderer);
    Windows_->GUI_Window_SystemLog_              = std::make_unique<GUI_Window_SystemLog>              (SystemUtils_);
    Windows_->GUI_Window_TestEditor_             = std::make_unique<GUI_Window_TestEditor>             (SystemUtils_);
    Windows_->GUI_Window_ThemeSelector_          = std::make_unique<GUI_Window_ThemeSelector>          (ThemeManager);
    Windows_->GUI_Window_FontSelector_           = std::make_unique<GUI_Window_FontSelector>           (FontManager);
    Windows_->GUI_Window_OpenProject_            = std::make_unique<GUI_Window_OpenProject>            (SystemUtils_);
    Windows_->GUI_Window_NewProject_             = std::make_unique<GUI_Window_NewProject>             (SystemUtils_);
    Windows_->GUI_Window_AssetStreamingSettings_ = std::make_unique<GUI_Window_AssetStreamingSettings> (SystemUtils_, ProjectUtils);
    Windows_->GUI_Window_ImportModel_            = std::make_unique<GUI_Window_ImportModel>            (SystemUtils_, ProjectUtils, Windows_->GUI_Window_ImportProgressBar_.get());
    Windows_->GUI_Window_ImportModelDirectory_   = std::make_unique<GUI_Window_ImportModelDirectory>   (SystemUtils_, ProjectUtils, Windows_->GUI_Window_ImportProgressBar_.get());
    

    SystemUtils_->Logger_->Log("WindowManager Subsystem Finished Setting Up Window Struct", 3);
    SystemUtils_->Logger_->Log("WindowManager Subsystem Setting Up Window Index", 4);

    WindowNames_.push_back("About");
    WindowNames_.push_back("AssetExplorer");
    WindowNames_.push_back("ControllerSettings");
    WindowNames_.push_back("FrameLatencyGraph");
    WindowNames_.push_back("FramerateGraph");
    WindowNames_.push_back("FramerateCounter");
    WindowNames_.push_back("FramerateHistogram");
    WindowNames_.push_back("FrameratePlot");
    WindowNames_.push_back("ImportProgressBar");
    WindowNames_.push_back("ObjectProperties");
    WindowNames_.push_back("ProjectSettings");
    WindowNames_.push_back("RAMGraph");
    WindowNames_.push_back("RenderingSettings");
    WindowNames_.push_back("EditorCameraSettings");
    WindowNames_.push_back("SceneTree");
    WindowNames_.push_back("ScriptEditor");
    WindowNames_.push_back("ShaderEditor");
    WindowNames_.push_back("SystemLog");
    WindowNames_.push_back("TestEditor");
    WindowNames_.push_back("ThemeSelector");
    WindowNames_.push_back("FontSelector");
    WindowNames_.push_back("OpenProject");
    WindowNames_.push_back("NewProject");
    WindowNames_.push_back("AssetStreamingSettings");
    WindowNames_.push_back("ImportModel");
    WindowNames_.push_back("ImportModelDirectory");
    
    
    SystemUtils_->Logger_->Log("WindowManager Subsystem Finished Setting Up Window Index", 3);



}

// Update Windows - See README in Windows Dir For More Info
void ERS_CLASS_WindowManager::UpdateAllWindows() {

    // Make sure to add your window here as well, again see the above mentioned readme for more info
    Windows_->GUI_Window_About_                   ->Draw();
    Windows_->GUI_Window_AssetExplorer_           ->Draw();
    Windows_->GUI_Window_ControllerSettings_      ->Draw();
    Windows_->GUI_Window_FrameLatencyGraph_       ->Draw();
    Windows_->GUI_Window_FramerateGraph_          ->Draw();
    Windows_->GUI_Window_FramerateCounter_        ->Draw();
    Windows_->GUI_Window_FramerateHistogram_      ->Draw();
    Windows_->GUI_Window_FrameratePlot_           ->Draw();
    Windows_->GUI_Window_ImportProgressBar_       ->Draw();
    Windows_->GUI_Window_ObjectProperties_        ->Draw();
    Windows_->GUI_Window_ProjectSettings_         ->Draw();
    Windows_->GUI_Window_RAMGraph_                ->Draw();
    Windows_->GUI_Window_RenderingSettings_       ->Draw();
    Windows_->GUI_Window_EditorCameraSettings_    ->Draw();
    Windows_->GUI_Window_SceneTree_               ->Draw();
    Windows_->GUI_Window_ScriptEditor_            ->Draw();
    Windows_->GUI_Window_ShaderEditor_            ->Draw();
    Windows_->GUI_Window_SystemLog_               ->Draw();
    Windows_->GUI_Window_TestEditor_              ->Draw();
    Windows_->GUI_Window_ThemeSelector_           ->Draw();
    Windows_->GUI_Window_FontSelector_            ->Draw();
    Windows_->GUI_Window_OpenProject_             ->Draw();
    Windows_->GUI_Window_NewProject_              ->Draw();
    Windows_->GUI_Window_AssetStreamingSettings_  ->Draw();
    Windows_->GUI_Window_ImportModel_             ->Draw();
    Windows_->GUI_Window_ImportModelDirectory_    ->Draw();
    

}

bool ERS_CLASS_WindowManager::SetWindowStatus(std::string WindowName, bool Status) {


    // Handle Different windows, make sure to update this as well when adding new windows
    if (WindowName == "About") {
        Windows_->GUI_Window_About_->Enabled_ = Status;
        return true;
    } else if (WindowName == "AssetExplorer") {
        Windows_->GUI_Window_AssetExplorer_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ControllerSettings") {
        Windows_->GUI_Window_ControllerSettings_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FrameLatencyGraph") {
        Windows_->GUI_Window_FrameLatencyGraph_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FramerateGraph") {
        Windows_->GUI_Window_FramerateGraph_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FramerateCounter") {
        Windows_->GUI_Window_FramerateCounter_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FramerateHistogram") {
        Windows_->GUI_Window_FramerateHistogram_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FrameratePlot") {
        Windows_->GUI_Window_FrameratePlot_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ImportProgressBar") {
        Windows_->GUI_Window_ImportProgressBar_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ObjectProperties") {
        Windows_->GUI_Window_ObjectProperties_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ProjectSettings") {
        Windows_->GUI_Window_ProjectSettings_->Enabled_ = Status;
        return true;
    } else if (WindowName == "RAMGraph") {
        Windows_->GUI_Window_RAMGraph_->Enabled_ = Status;
        return true;
    } else if (WindowName == "RenderingSettings") {
        Windows_->GUI_Window_RenderingSettings_->Enabled_ = Status;
        return true;
    } else if (WindowName == "EditorCameraSettings") {
        Windows_->GUI_Window_EditorCameraSettings_->Enabled_ = Status;
        return true;
    } else if (WindowName == "SceneTree") {
        Windows_->GUI_Window_SceneTree_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ScriptEditor") {
        Windows_->GUI_Window_ScriptEditor_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ShaderEditor") {
        Windows_->GUI_Window_ShaderEditor_->Enabled_ = Status;
        return true;
    } else if (WindowName == "SystemLog") {
        Windows_->GUI_Window_SystemLog_->Enabled_ = Status;
        return true;
    } else if (WindowName == "TestEditor") {
        Windows_->GUI_Window_TestEditor_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ThemeSelector") {
        Windows_->GUI_Window_ThemeSelector_->Enabled_ = Status;
        return true;
    } else if (WindowName == "FontSelector") {
        Windows_->GUI_Window_FontSelector_->Enabled_ = Status;
        return true;
    } else if (WindowName == "OpenProject") {
        Windows_->GUI_Window_OpenProject_->Enabled_ = Status;
        return true;
    } else if (WindowName == "NewProject") {
        Windows_->GUI_Window_NewProject_->Enabled_ = Status;
        return true;
    } else if (WindowName == "AssetStreamingSettings") {
        Windows_->GUI_Window_AssetStreamingSettings_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ImportModel") {
        Windows_->GUI_Window_ImportModel_->Enabled_ = Status;
        return true;
    } else if (WindowName == "ImportModelDirectory") {
        Windows_->GUI_Window_ImportModelDirectory_->Enabled_ = Status;
        return true;
    } else {
        return false;
    }


}

bool ERS_CLASS_WindowManager::GetWindowStatus(std::string WindowName, bool* Status) {

    // Handle Different windows, make sure to update this as well when adding new windows
    if (WindowName == "About") {
        *Status = Windows_->GUI_Window_About_->Enabled_;
        return true;
    } else if (WindowName == "AssetExplorer") {
        *Status = Windows_->GUI_Window_AssetExplorer_->Enabled_;
        return true;
    } else if (WindowName == "ControllerSettings") {
        *Status = Windows_->GUI_Window_ControllerSettings_->Enabled_;
        return true;
    } else if (WindowName == "FrameLatencyGraph") {
        *Status = Windows_->GUI_Window_FrameLatencyGraph_->Enabled_;
        return true;
    } else if (WindowName == "FramerateGraph") {
        *Status = Windows_->GUI_Window_FramerateGraph_->Enabled_;
        return true;
    } else if (WindowName == "FramerateCounter") {
        *Status = Windows_->GUI_Window_FramerateCounter_->Enabled_;
        return true;
    } else if (WindowName == "FramerateHistogram") {
        *Status = Windows_->GUI_Window_FramerateHistogram_->Enabled_;
        return true;
    } else if (WindowName == "FrameratePlot") {
        *Status = Windows_->GUI_Window_FrameratePlot_->Enabled_;
        return true;
    } else if (WindowName == "ImportProgressBar") {
        *Status = Windows_->GUI_Window_ImportProgressBar_->Enabled_;
        return true;
    } else if (WindowName == "ObjectProperties") {
        *Status = Windows_->GUI_Window_ObjectProperties_->Enabled_;
        return true;
    } else if (WindowName == "ProjectSettings") {
        *Status = Windows_->GUI_Window_ProjectSettings_->Enabled_;
        return true;
    } else if (WindowName == "RAMGraph") {
        *Status = Windows_->GUI_Window_RAMGraph_->Enabled_;
        return true;
    } else if (WindowName == "RenderingSettings") {
        *Status = Windows_->GUI_Window_RenderingSettings_->Enabled_;
        return true;
    } else if (WindowName == "EditorCameraSettings") {
        *Status = Windows_->GUI_Window_EditorCameraSettings_->Enabled_;
        return true;
    } else if (WindowName == "SceneTree") {
        *Status = Windows_->GUI_Window_SceneTree_->Enabled_;
        return true;
    } else if (WindowName == "ScriptEditor") {
        *Status = Windows_->GUI_Window_ScriptEditor_->Enabled_;
        return true;
    } else if (WindowName == "ShaderEditor") {
        *Status = Windows_->GUI_Window_ShaderEditor_->Enabled_;
        return true;
    } else if (WindowName == "SystemLog") {
        *Status = Windows_->GUI_Window_SystemLog_->Enabled_;
        return true;
    } else if (WindowName == "TestEditor") {
        *Status = Windows_->GUI_Window_TestEditor_->Enabled_;
        return true;
    } else if (WindowName == "ThemeSelector") {
        *Status = Windows_->GUI_Window_ThemeSelector_->Enabled_;
        return true;
    } else if (WindowName == "FontSelector") {
        *Status = Windows_->GUI_Window_FontSelector_->Enabled_;
        return true;
    } else if (WindowName == "OpenProject") {
        *Status = Windows_->GUI_Window_OpenProject_->Enabled_;
        return true;
    } else if (WindowName == "NewProject") {
        *Status = Windows_->GUI_Window_NewProject_->Enabled_;
        return true;
    } else if (WindowName == "AssetStreamingSettings") {
        *Status = Windows_->GUI_Window_AssetStreamingSettings_->Enabled_;
        return true;
    } else if (WindowName == "ImportModel") {
        *Status = Windows_->GUI_Window_ImportModel_->Enabled_;
        return true;
    } else if (WindowName == "ImportModelDirectory") {
        *Status = Windows_->GUI_Window_ImportModelDirectory_->Enabled_;
        return true;
    } else {
        return false;
    }

}

std::vector<std::string> ERS_CLASS_WindowManager::GetWindowNames() {
    return WindowNames_;
}

ERS_STRUCT_Windows* ERS_CLASS_WindowManager::GetWindowsStruct() {
    return Windows_.get();
}