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


/**
 * @brief Create GUI Window "Rendering Settings"
 * 
 */
class Subwindow_ModelRenameModal {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<Model Manager Instance Pointer*/
    char ModelInputName_[128] = ""; /**<Character Array To Store Model Name When Renaming*/


public:

    // Show/Hide Var
    bool Enabled_ = false; /**<Show/Hide Window*/
    bool FirstFrame_ = false; /**<Used To Run Certain Things On Window Open*/
    int SelectedModel_ = 0; /**<Indicate What Index The Selected Model Is*/
    int SelectedScene_ = 0; /**<Indicate What Index The Selected Scene Is*/

public:

    /**
     * @brief Construct a new Subwindow_ModelRenameModal object
     * 
     */
    Subwindow_ModelRenameModal(ERS_CLASS_SceneManager* SceneManager);

    /**
     * @brief Destroy the Subwindow_ModelRenameModal object
     * 
     */
    ~Subwindow_ModelRenameModal();


    /**
     * @brief Show The Model Rename Modal
     * 
     * @param SelectedScene
     * @param SelectedModel
     */
    void Activate(int SelectedScene, int SelectedModel);


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};