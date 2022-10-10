//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Create GUI GUI_Window "Framerate Counter"
 * 
 */
class GUI_Window_FramerateCounter {

public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};