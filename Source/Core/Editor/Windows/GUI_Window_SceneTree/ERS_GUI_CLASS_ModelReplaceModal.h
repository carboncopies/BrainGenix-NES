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
class Subwindow_ModelReplaceModal {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<Model Manager Instance Pointer*/
    char ModelInputName_[128] = ""; /**<Character Array To Store Model Name When Renaming*/


public:

    // Show/Hide Var
    bool Enabled_ = false; /**<Show/Hide Window*/
    bool FirstFrame_ = false; /**<Used To Run Certain Things On Window Open*/
    unsigned long SelectedModel_ = 0; /**<Indicate What Index The Selected Model Is*/
    unsigned long SelectedScene_ = 0; /**<Indicate What Index The Selected Scene Is*/
    unsigned long ReplacementModelIndex_ = 0; /**<Index of the model we're going to replace the selected model with*/

public:

    /**
     * @brief Construct a new Subwindow_ModelReplaceModal object
     * 
     */
    Subwindow_ModelReplaceModal(ERS_CLASS_SceneManager* SceneManager);

    /**
     * @brief Destroy the Subwindow_ModelReplaceModal object
     * 
     */
    ~Subwindow_ModelReplaceModal();


    /**
     * @brief Show The Model Replace Modal
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