//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_Scene.h>


/**
 * @brief Create GUI Window "Rendering Settings"
 * 
 */
class Subwindow_SceneRenameModal {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<Scene Manager Instance Pointer*/
    char SceneInputName_[128] = ""; /**<Character Array To Store Scene Name When Renaming*/


public:

    // Show/Hide Var
    bool Enabled_ = false; /**<Show/Hide Window*/
    bool FirstFrame_ = false; /**<Used To Run Certain Things On Window Open*/
    int SelectedScene_ = 0; /**<Indicate What Index The Selected Scene Is*/

public:

    /**
     * @brief Construct a new Subwindow_SceneRenameModal object
     * 
     */
    Subwindow_SceneRenameModal(ERS_CLASS_SceneManager* SceneManager);

    /**
     * @brief Destroy the Subwindow_SceneRenameModal object
     * 
     */
    ~Subwindow_SceneRenameModal();


    /**
     * @brief Show The Scene Rename Modal
     * 
     * @param SelectedScene 
     */
    void Activate(int SelectedScene);


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};