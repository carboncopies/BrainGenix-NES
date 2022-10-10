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
#include <ERS_STRUCT_HardwareInfo.h>

/**
 * @brief Create GUI GUI_Window "Framerate Graph"
 * 
 */
class GUI_Window_RAMGraph {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Pointer*/

    std::vector<float> TotalRAM_; /**<Total Amount Of RAM In GB In The System*/
    std::vector<float> FreeRAM_; /**<Total Amount Of Free RAM In The System*/
    std::vector<float> TotalSwap_; /**<Total Amount Of Swap In The System*/
    std::vector<float> FreeSwap_; /**<Total Amount Of Free Swap In The SYstem*/


public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/

public:

    /**
     * @brief Construct a new GUI_Window_RAMGraph object
     * 
     * @param SystemUtils 
     */
    GUI_Window_RAMGraph(ERS_STRUCT_SystemUtils* SystemUtils);
    
    /**
     * @brief Destroy the GUI_Window_RAMGraph object
     * 
     */
    ~GUI_Window_RAMGraph();


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();


};