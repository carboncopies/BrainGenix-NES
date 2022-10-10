//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_CLASS_VisualRenderer.h>

#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_Windows.h>

#include <GUI_Window_SceneTree.h>
#include <GUI_Window_SystemLog.h>
#include <GUI_Window_AssetExplorer.h>
#include <GUI_Window_ScriptEditor.h>

#include <GUI_Window_RenderingSettings.h>
#include <GUI_Window_RAMGraph.h>
#include <GUI_Window_ObjectProperties.h>
#include <GUI_Window_FramerateGraph.h>
#include <GUI_Window_FrameLatencyGraph.h>
#include <GUI_Window_FramerateCounter.h>
#include <GUI_Window_FramerateHistogram.h>
#include <GUI_Window_FrameratePlot.h>
#include <GUI_Window_ShaderEditor.h>
#include <GUI_Window_AssetStreamingSettings.h>


#include <ERS_Editor_3DCursor.h>
#include <ERS_SceneManager.h>


/**
 * @brief This class provides the "File" Menu In The Editor.
 * 
 */
class GUI_Menu_Window {


private:

    ERS_STRUCT_SystemUtils*   SystemUtils_    = nullptr; /**<SystemUtils Instance, Used To Get Systems From Other Classes*/
    ERS_STRUCT_Windows*       Windows_        = nullptr; /**<WIndows struct*/
    ERS_CLASS_VisualRenderer* VisualRenderer_ = nullptr; /**<ERS_CLASS_VisualRenderer instance*/


public:


    /**
     * @brief Construct a new gui menu object
     * 
     * @param SystemUtils 
     */
    GUI_Menu_Window(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Windows* Windows, ERS_CLASS_VisualRenderer* VisualRendererInstance);


    /**
     * @brief Destroy the gui menu object
     * 
     */
    ~GUI_Menu_Window();


    /**
     * @brief This function Draws The View Menu Contents.
     * 
     */
    void Draw();


};