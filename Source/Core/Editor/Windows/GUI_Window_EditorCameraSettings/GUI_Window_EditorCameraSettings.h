//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <algorithm>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_RendererSettings.h>

#include <ERS_ENUM_ShadowFilteringType.h>
#include <ERS_ENUM_ShadowUpdateMode.h>

#include <ERS_CLASS_VisualRenderer.h>


/**
 * @brief Create GUI GUI_Window "EditorCamera Settings"
 * 
 */
class GUI_Window_EditorCameraSettings {

private:

    ERS_STRUCT_SystemUtils*    SystemUtils_;    /**<System Utils Pointer*/
    ERS_CLASS_VisualRenderer*  VisualRenderer_; /**<Pointer to visual renderer subsystem*/
    int SelectedViewportIndex_ = 0;            /**<Index of the viewport which is to be shown*/

public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/


    /**
     * @brief Construct a new GUI_Window_EditorCameraSettings object
     * 
     * @param SystemUtils 
     */
    GUI_Window_EditorCameraSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_VisualRenderer* VisualRenderer);

    /**
     * @brief Destroy the GUI_Window_EditorCameraSettings object
     * 
     */
    ~GUI_Window_EditorCameraSettings();


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};