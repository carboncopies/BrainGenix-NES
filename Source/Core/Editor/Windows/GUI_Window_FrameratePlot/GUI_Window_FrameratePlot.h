//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Create GUI GUI_Window "Framerate Plot"
 * 
 */
class GUI_Window_FrameratePlot {

private:

    std::vector<float> FramerateHistory_; /**<Vector To Store Framerate histories*/ 
    int HistoryLength_ = 1000; /**<Set Framerate History Length*/

public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};