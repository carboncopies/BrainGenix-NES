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
#include <yaml-cpp/yaml.h>

#include <imgui.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>


#include <ERS_ProjectLoader.h>
#include <ERS_SceneWriter.h>
#include <ERS_SceneManager.h>

#include <ERS_Editor_WindowManager.h>

#include <GUI_Window_ProjectSettings.h>

#include <ERS_CLASS_VisualRenderer.h>


/**
 * @brief This class provides the "File" Menu In The Editor.
 * 
 */
class GUI_Menu_File {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_      = nullptr; /**<SystemUtils Instance, Used To Get Systems From Other Classes*/
    ERS_CLASS_VisualRenderer* VisualRenderer_ = nullptr; /**<shared pointer to visual renderer*/
    ERS_STRUCT_Windows* Windows_              = nullptr; /**<Struct containing all windows*/
    ERS_CLASS_SceneManager* SceneManager_     = nullptr; /**<Scene Manager Instance Pointer*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_    = nullptr; /**<Pointer To Project Utils Instance*/
    std::unique_ptr<SceneWriter> SceneWriter_;           /**<Scene Writer Instance Pointer*/

public:


    /**
     * @brief Construct a new gui menu file object
     * 
     * @param SystemUtils 
     */
    GUI_Menu_File(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_Windows* Windows);


    /**
     * @brief Destroy the gui menu file object
     * 
     */
    ~GUI_Menu_File();


    /**
     * @brief This function Draws The File Menu Contents.
     * 
     */
    void Draw();

};