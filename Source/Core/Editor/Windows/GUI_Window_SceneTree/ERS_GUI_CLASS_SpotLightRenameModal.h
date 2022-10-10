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
class Subwindow_SpotLightRenameModal {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<SpotLight Manager Instance Spoter*/
    char SpotLightInputName_[128] = ""; /**<Character Array To Store SpotLight Name When Renaming*/


public:

    // Show/Hide Var
    bool Enabled_ = false; /**<Show/Hide Window*/
    bool FirstFrame_ = false; /**<Used To Run Certain Things On Window Open*/
    int SelectedSpotLight_ = 0; /**<Indicate What Index The Selected SpotLight Is*/
    int SelectedScene_ = 0; /**<Indicate What Index The Selected Scene Is*/

public:

    /**
     * @brief Construct a new Subwindow_SpotLightRenameModal object
     * 
     */
    Subwindow_SpotLightRenameModal(ERS_CLASS_SceneManager* SceneManager);

    /**
     * @brief Destroy the Subwindow_SpotLightRenameModal object
     * 
     */
    ~Subwindow_SpotLightRenameModal();


    /**
     * @brief Show The SpotLight Rename Modal
     * 
     * @param SelectedScene
     * @param SelectedSpotLight
     */
    void Activate(int SelectedScene, int SelectedSpotLight);


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};