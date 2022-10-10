//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_RendererSettings.h>

#include <ERS_ENUM_ShadowFilteringType.h>
#include <ERS_ENUM_ShadowUpdateMode.h>



/**
 * @brief Create GUI GUI_Window "Rendering Settings"
 * 
 */
class GUI_Window_RenderingSettings {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Pointer*/

    ImVec4 ClearColor_ = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); /**<Background Clearing Color */
    bool OpenGLDrawLines_ = false; /**<Default Rendering Mode */

    // Variables To Store Input Data
    int DepthMapResolution_; /**>Stores the value that the user is putting into the depth map input box*/
    int SelectedShadowFiltering_ = 0;
    int SelectedShadowUpdates_ = 0;

public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/


    /**
     * @brief Construct a new GUI_Window_RenderingSettings object
     * 
     * @param SystemUtils 
     */
    GUI_Window_RenderingSettings(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the GUI_Window_RenderingSettings object
     * 
     */
    ~GUI_Window_RenderingSettings();


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};