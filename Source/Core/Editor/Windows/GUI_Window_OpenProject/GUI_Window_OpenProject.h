//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <imgui.h>

#include <ImGuiFileDialog.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>


/**
 * @brief This class provides the GUI to the import asset option within the file menu.
 *
 */
class GUI_Window_OpenProject {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<used to get access to system utilites like IOmanager, logger, etc.*/
    bool LastWindowState_ = false; /**<State of the wiundow last frame*/

public:

    bool Enabled_ = false; /**<Show/hide the window*/

    /**
     * @brief Construct a new gui importasset object.
     * 
     * @param SystemUtils 
     */
    GUI_Window_OpenProject(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the gui importasset object.
     * 
     */
    ~GUI_Window_OpenProject();

    /**
     * @brief Update Any Windows
     * 
     */
    void Draw();

};