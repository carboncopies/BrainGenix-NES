//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>
#include <implot.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>

/**
 * @brief Create GUI GUI_Window "Framerate Graph"
 * 
 */
class GUI_Window_FrameLatencyGraph {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Pointer*/

public:

    bool Enabled_ = true; /**<Show/Hide GUI_Window*/

public:

    /**
     * @brief Construct a new GUI_Window_FrameLatencyGraph object
     * 
     * @param SystemUtils 
     */
    GUI_Window_FrameLatencyGraph(ERS_STRUCT_SystemUtils* SystemUtils);
    
    /**
     * @brief Destroy the GUI_Window_FrameLatencyGraph object
     * 
     */
    ~GUI_Window_FrameLatencyGraph();


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();


};