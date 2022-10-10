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
class GUI_Window_ImportProgressBar {

private:

    int CurrentAssetNumber_ = 0; /**<Set Current Number Of Asset Being Imported*/
    int TotalAssetsToImport_ = 1; /**<Set Total Number Of Assets To Import*/

    int ConsecFinished_ = 0; /**<Set number of consec frames that job is finished*/
    int ConsecFinishedThreshold_ = 60; /**<Hide After 100 frames of finished job*/
    
    float PercentDone_; /**<Float Used By Progress Bar*/
    bool IsJobFinishing_; /**<Bool Set By Thread*/

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointers To System Resources*/

public:

    bool Enabled_ = false; /**<Is Popup Enabled*/


public:

    /**
     * @brief Construct a new Window_ImportProgressBar object
     * 
     */
    GUI_Window_ImportProgressBar(ERS_STRUCT_SystemUtils* SystemUtils);
    
    /**
     * @brief Destroy the Window_ImportProgressBar object
     * 
     */
    ~GUI_Window_ImportProgressBar();


    /**
     * @brief Update Total items being processed (progress bar)
     * 
     * @param Current 
     * @param Total 
     */
    void UpdateTotalItems(long Current, long Total);

    /**
     * @brief close the window a few frames after the job finishes
     * 
     * @param JobState 
     */
    void UpdateJobState(bool JobFinished);

    /**
     * @brief Draw import bar if needed (call every frame)
     * 
     */
    void Draw();

};