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
#include <ERS_STRUCT_ProjectUtils.h>

#include <ERS_GUI_CLASS_ScriptRenameModal.h>


/**
 * @brief Window for the Import progress bar.
 * 
 */
class GUI_Window_AssetExplorer {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointers To System Resources*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Pointer to project info struct*/

    std::unique_ptr<Subwindow_ScriptRenameModal> Subwindow_ScriptRenameModal_; /**<Script Rename Modal*/ 

    std::vector<bool> AssetIDSelectionList_; /**<Select Multiple Asset IDs*/
    unsigned long LastSelectedIndex_ = 0; /**<Index of the last item to be clicked on*/
    unsigned long SelectedModelIndex_ = 0; /**<Index Of Selected Model*/
    unsigned long SelectedScriptIndex_ = 0; /**<Index of selected script*/

public:

    bool Enabled_ = true; /**<Is Popup Enabled*/
    bool AdvancedMode_ = true; /**<Toggles on/off Advanced Mode*/

public:

    /**
     * @brief Construct a new Window_AssetExplorer object
     * 
     */
    GUI_Window_AssetExplorer(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils);
    
    /**
     * @brief Destroy the Window_AssetExplorer object
     * 
     */
    ~GUI_Window_AssetExplorer();


    /**
     * @brief Draw system log needed (call every frame)
     * 
     */
    void Draw();

};