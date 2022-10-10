//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_Scene.h>

#include <ERS_Editor_3DCursor.h>


/**
 * @brief Implement Scene Deltion, Open Popup For Confirm, Error.
 * 
 */
class Subwindow_DeleteModel {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<SceneManager Instance*/
    Cursors3D* Cursor_; /**<Instance of the 3d cursor object*/
    
    int SceneIndex_ = 0; /**<Scene Index Used To Remember What Scene To Delete From*/
    int ModelIndex_ = 0; /**<Use to identify what model to delete*/

    bool FirstFrame_ = false; /**<Used to grab keyboard input on first frame window is open*/

    bool ShowDeleteConfirm_ = false; /**<Confirm Deletion Of Scene*/

private:

    void UpdateConfirmDeletePopup(); /**<Function To Update Popup Deletion Window*/

public:

    /**
     * @brief Construct a new Subwindow_DeleteModel object
     * 
     */
    Subwindow_DeleteModel(ERS_CLASS_SceneManager* SceneManager, Cursors3D* Cursor);

    /**
     * @brief Destroy the Subwindow_DeleteModel object
     * 
     */
    ~Subwindow_DeleteModel();

    /**
     * @brief Called by the GUI to delete a scene.
     * 
     * @param SceneIndex 
     * @param ModelIndex 
     */
    void DeleteModel(int SceneIndex, int ModelIndex);

    /**
     * @brief Update all windows created by this class. Call every frame.
     * 
     */
    void Draw();

};

 