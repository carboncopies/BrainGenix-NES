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
class Subwindow_DirectionalLightRenameModal {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<DirectionalLight Manager Instance Directionaler*/
    char DirectionalLightInputName_[128] = ""; /**<Character Array To Store DirectionalLight Name When Renaming*/


public:

    // Show/Hide Var
    bool Enabled_ = false; /**<Show/Hide Window*/
    bool FirstFrame_ = false; /**<Used To Run Certain Things On Window Open*/
    int SelectedDirectionalLight_ = 0; /**<Indicate What Index The Selected DirectionalLight Is*/
    int SelectedScene_ = 0; /**<Indicate What Index The Selected Scene Is*/

public:

    /**
     * @brief Construct a new Subwindow_DirectionalLightRenameModal object
     * 
     */
    Subwindow_DirectionalLightRenameModal(ERS_CLASS_SceneManager* SceneManager);

    /**
     * @brief Destroy the Subwindow_DirectionalLightRenameModal object
     * 
     */
    ~Subwindow_DirectionalLightRenameModal();


    /**
     * @brief Show The DirectionalLight Rename Modal
     * 
     * @param SelectedScene
     * @param SelectedDirectionalLight
     */
    void Activate(int SelectedScene, int SelectedDirectionalLight);


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};