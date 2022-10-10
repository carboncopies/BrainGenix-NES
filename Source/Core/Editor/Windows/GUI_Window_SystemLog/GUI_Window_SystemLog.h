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


/**
 * @brief Window for the Import progress bar.
 * 
 */
class GUI_Window_SystemLog {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointers To System Resources*/
    int MinLogLevel_ = 0; /**<Default Min Log Level*/
    int StartingLogIndex_ = 0; /**<Starting Log Index, Set To Current Length To Clear*/
    bool AutoScroll_ = true; /**<Autoscroll Log*/

public:

    bool Enabled_ = false; /**<Is Popup Enabled*/


public:

    /**
     * @brief Construct a new Window_SystemLog object
     * 
     */
    GUI_Window_SystemLog(ERS_STRUCT_SystemUtils* SystemUtils);
    
    /**
     * @brief Destroy the Window_SystemLog object
     * 
     */
    ~GUI_Window_SystemLog();


    /**
     * @brief Draw system log needed (call every frame)
     * 
     */
    void Draw();

};