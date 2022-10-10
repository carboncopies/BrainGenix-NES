//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_Editor_FontManager.h>


/**
 * @brief This Class Provides The Font Selector Window.
 * 
 */
class GUI_Window_FontSelector {

private:

    ERS_CLASS_FontManager* FontManager_; /**<Font Manager Instance*/


public:

    bool Enabled_ = false; /**<Show/Hide Window*/


public:

    /**
     * @brief Construct a new Window_FontSelector object
     * 
     * @param FontManager 
     */
    GUI_Window_FontSelector(ERS_CLASS_FontManager* FontManager);

    /**
     * @brief Destroy the Window_FontSelector object
     * 
     */
    ~GUI_Window_FontSelector();

    /**
     * @brief Update the window contents, Call This Every Frame.
     * 
     */
    void Draw();



};