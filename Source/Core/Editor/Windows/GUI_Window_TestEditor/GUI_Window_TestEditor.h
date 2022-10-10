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
#include <TextEditor.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>



/**
 * @brief This class provides the window for a test editor, which doesn't actually do anything other than let you type into it to test zep.
 * 
 */
class GUI_Window_TestEditor {


private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointer to system utils struct*/
    TextEditor Editor_; /** Editor Instance*/

public:

    bool Enabled_ = false; /**<Control var for the window, enables or disables it.*/


public:

    /**
     * @brief Construct a new Window_TestEditor object
     * 
     * @param SystemUtils 
     */
    GUI_Window_TestEditor(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the Window_TestEditor object
     * 
     */
    ~GUI_Window_TestEditor();


    /**
     * @brief Call every frame to update the window.
     * 
     */
    void Draw();

};