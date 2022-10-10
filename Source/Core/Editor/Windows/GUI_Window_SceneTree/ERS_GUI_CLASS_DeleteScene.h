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
class Subwindow_DeleteScene {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<SceneManager Instance*/
    Cursors3D* Cursor_; /**<Instance of the 3d cursor object*/

    
    int SceneIndex_ = 0; /**<SceneIndex Used To Remember What Scene To Delete*/

    bool FirstFrame_ = false; /**<Used to grab keyboard input on first frame window is open*/

    bool ShowDeleteConfirm_ = false; /**<Confirm Deletion Of Scene*/
    bool ShowOneSceneDeleteError_ = false; /**<Error popup that shows when one scene is remaining, (Can't have no scenes)*/

private:

    void UpdateConfirmDeletePopup(); /**<Function To Update Popup Deletion Window*/
    void UpdateOneSceneDeleteErrorPopup(); /**<Last scene deletion error popup*/

public:

    /**
     * @brief Construct a new Subwindow_DeleteScene object
     * 
     */
    Subwindow_DeleteScene(ERS_CLASS_SceneManager* SceneManager, Cursors3D* Cursor);

    /**
     * @brief Destroy the Subwindow_DeleteScene object
     * 
     */
    ~Subwindow_DeleteScene();

    /**
     * @brief Called by the GUI to delete a scene.
     * 
     * @param SceneIndex 
     */
    void DeleteScene(int SceneIndex);

    /**
     * @brief Update all windows created by this class. Call every frame.
     * 
     */
    void Draw();

};
