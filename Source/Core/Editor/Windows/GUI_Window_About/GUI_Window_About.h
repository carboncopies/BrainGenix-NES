//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <thread>
#include <string>
#include <mutex>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>

#include <ERS_Version.h>
#include <ERS_CompilePlatformInfo.h>
#include <ERS_CompileTimeStamp.h>


/**
 * @brief Window for the Import progress bar.
 * 
 */
class GUI_Window_About {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointers To System Resources*/

public:

    bool Enabled_ = false; /**<Is Popup Enabled*/


public:

    /**
     * @brief Construct a new GUI_Window_About object
     * 
     */
    GUI_Window_About(ERS_STRUCT_SystemUtils* SystemUtils);
    
    /**
     * @brief Destroy the GUI_Window_About object
     * 
     */
    ~GUI_Window_About();


    /**
     * @brief Draw import bar if needed (call every frame)
     * 
     */
    void Draw();

};