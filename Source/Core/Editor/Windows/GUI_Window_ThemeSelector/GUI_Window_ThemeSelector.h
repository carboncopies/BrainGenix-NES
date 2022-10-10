//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_Editor_ThemeManager.h>


/**
 * @brief This Class Provides The Theme Selector Window.
 * 
 */
class GUI_Window_ThemeSelector {

private:

    ERS_CLASS_ThemeManager* ThemeManager_; /**<Theme Manager Instance*/


public:

    bool Enabled_ = false; /**<Show/Hide Window*/


public:

    /**
     * @brief Construct a new Window_ThemeSelector object
     * 
     * @param ThemeManager 
     */
    GUI_Window_ThemeSelector(ERS_CLASS_ThemeManager* ThemeManager);

    /**
     * @brief Destroy the Window_ThemeSelector object
     * 
     */
    ~GUI_Window_ThemeSelector();

    /**
     * @brief Update the window contents, Call This Every Frame.
     * 
     */
    void Draw();



};