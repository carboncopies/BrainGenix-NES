//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <GUI_Window_About.h>
#include <GUI_Window_AssetExplorer.h>
#include <GUI_Window_ControllerSettings.h>
#include <GUI_Window_FrameLatencyGraph.h>
#include <GUI_Window_FramerateCounter.h>
#include <GUI_Window_FramerateGraph.h>
#include <GUI_Window_FramerateHistogram.h>
#include <GUI_Window_FrameratePlot.h>
#include <GUI_Window_ImportProgressBar.h>
#include <GUI_Window_ObjectProperties.h>
#include <GUI_Window_ProjectSettings.h>
#include <GUI_Window_RAMGraph.h>
#include <GUI_Window_RenderingSettings.h>
#include <GUI_Window_EditorCameraSettings.h>
#include <GUI_Window_SceneTree.h>
#include <GUI_Window_ScriptEditor.h>
#include <GUI_Window_ShaderEditor.h>
#include <GUI_Window_SystemLog.h>
#include <GUI_Window_TestEditor.h>
#include <GUI_Window_ThemeSelector.h>
#include <GUI_Window_FontSelector.h>
#include <GUI_Window_OpenProject.h>
#include <GUI_Window_NewProject.h>
#include <GUI_Window_AssetStreamingSettings.h>

#include <GUI_Window_ImportModel.h>
#include <GUI_Window_ImportModelDirectory.h>


/**
 * @brief This structure holds unique pointers to all windows that are instantiated by the GUI.
 * When adding a new window, add a unique_ptr here and update the WindowManager as well so that it's updated every frame.
 * 
 */
struct ERS_STRUCT_Windows {

    // Pointers to all Window Objects
    std::unique_ptr<GUI_Window_About>                  GUI_Window_About_;
    std::unique_ptr<GUI_Window_AssetExplorer>          GUI_Window_AssetExplorer_;
    std::unique_ptr<GUI_Window_ControllerSettings>     GUI_Window_ControllerSettings_;
    std::unique_ptr<GUI_Window_FrameLatencyGraph>      GUI_Window_FrameLatencyGraph_;
    std::unique_ptr<GUI_Window_FramerateCounter>       GUI_Window_FramerateCounter_;
    std::unique_ptr<GUI_Window_FramerateGraph>         GUI_Window_FramerateGraph_;
    std::unique_ptr<GUI_Window_FramerateHistogram>     GUI_Window_FramerateHistogram_;
    std::unique_ptr<GUI_Window_FrameratePlot>          GUI_Window_FrameratePlot_;
    std::unique_ptr<GUI_Window_ImportProgressBar>      GUI_Window_ImportProgressBar_;
    std::unique_ptr<GUI_Window_ObjectProperties>       GUI_Window_ObjectProperties_;
    std::unique_ptr<GUI_Window_ProjectSettings>        GUI_Window_ProjectSettings_;
    std::unique_ptr<GUI_Window_RAMGraph>               GUI_Window_RAMGraph_;
    std::unique_ptr<GUI_Window_RenderingSettings>      GUI_Window_RenderingSettings_;
    std::unique_ptr<GUI_Window_EditorCameraSettings>   GUI_Window_EditorCameraSettings_;
    std::unique_ptr<GUI_Window_SceneTree>              GUI_Window_SceneTree_;
    std::unique_ptr<GUI_Window_ScriptEditor>           GUI_Window_ScriptEditor_;
    std::unique_ptr<GUI_Window_ShaderEditor>           GUI_Window_ShaderEditor_;
    std::unique_ptr<GUI_Window_SystemLog>              GUI_Window_SystemLog_;
    std::unique_ptr<GUI_Window_TestEditor>             GUI_Window_TestEditor_;
    std::unique_ptr<GUI_Window_ThemeSelector>          GUI_Window_ThemeSelector_;
    std::unique_ptr<GUI_Window_FontSelector>           GUI_Window_FontSelector_;
    std::unique_ptr<GUI_Window_OpenProject>            GUI_Window_OpenProject_;
    std::unique_ptr<GUI_Window_NewProject>             GUI_Window_NewProject_;
    std::unique_ptr<GUI_Window_AssetStreamingSettings> GUI_Window_AssetStreamingSettings_;

    std::unique_ptr<GUI_Window_ImportModel>            GUI_Window_ImportModel_;
    std::unique_ptr<GUI_Window_ImportModelDirectory>   GUI_Window_ImportModelDirectory_;
    

};