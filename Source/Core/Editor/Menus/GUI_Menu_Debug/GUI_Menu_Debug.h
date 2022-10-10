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
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Windows.h>

#include <ERS_Editor_WindowManager.h>

#include <ERS_CLASS_OpenGLDebug.h>


/**
 * @brief This class provides the "File" Menu In The Editor.
 * 
 */
class GUI_Menu_Debug {


private:

    ERS_STRUCT_SystemUtils*                SystemUtils_           = nullptr; /**<SystemUtils Instance, Used To Get Systems From Other Classes*/
    ERS_STRUCT_Windows*                    Windows_               = nullptr; /**<Windows struct*/
    ERS_CLASS_WindowManager*               WindowManager_         = nullptr; /**<Used for window debugging, controls all windows systemwide*/
    std::unique_ptr<ERS_CLASS_OpenGLDebug> ERS_CLASS_OpenGLDebug_;           /**<Class used to draw the opengl debug menu into the main debug menu*/

    bool ShowImGuiDemoWindow_ = false; /**<Show/hide ImGui DemoWindow*/
    bool DebugMenuEnabled_ = false; /**Enable/Disable Debug Menu*/


public:


    /**
     * @brief Construct a new gui menu object
     * 
     * @param SystemUtils 
     */
    GUI_Menu_Debug(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Windows* Windows, ERS_CLASS_WindowManager* WindowManager);


    /**
     * @brief Destroy the gui menu object
     * 
     */
    ~GUI_Menu_Debug();


    /**
     * @brief This function Draws The View Menu Contents.
     * 
     */
    void Draw();


};