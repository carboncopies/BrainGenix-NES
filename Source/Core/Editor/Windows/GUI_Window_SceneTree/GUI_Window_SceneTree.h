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
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SceneObject.h>
#include <ERS_Editor_3DCursor.h>

#include <ERS_GUI_CLASS_SceneRenameModal.h>
#include <ERS_GUI_CLASS_ModelRenameModal.h>
#include <ERS_GUI_CLASS_PointLightRenameModal.h>
#include <ERS_GUI_CLASS_SpotLightRenameModal.h>
#include <ERS_GUI_CLASS_DirectionalLightRenameModal.h>

#include <ERS_GUI_CLASS_DeleteScene.h>
#include <ERS_GUI_CLASS_DeleteModel.h>
#include <ERS_GUI_CLASS_DeletePointLight.h>
#include <ERS_GUI_CLASS_DeleteDirectionalLight.h>
#include <ERS_GUI_CLASS_DeleteSpotLight.h>

#include <ERS_GUI_CLASS_ModelReplaceModal.h>

#include <ERS_GUI_FUNCTION_DuplicateScene.h>
#include <ERS_GUI_FUNCTION_DuplicateModel.h>
#include <ERS_GUI_FUNCTION_DuplicatePointLight.h>
#include <ERS_GUI_FUNCTION_DuplicateDirectionalLight.h>
#include <ERS_GUI_FUNCTION_DuplicateSpotLight.h>



/**
 * @brief Create GUI Window "Rendering Settings"
 * 
 */
class GUI_Window_SceneTree {

private:

    ERS_CLASS_SceneManager* SceneManager_; /**<Scene Manager Instance Pointer*/
    ERS_STRUCT_SystemUtils* SystemUtils_; /**<SystemUtils Pointer*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<ProjectUtils Pointer*/
    Cursors3D* Cursors3D_; /**<Pointer to Cursors3D Instance*/
    bool FirstFrame_ = true; /**<Bool Indicating if It's the first frame*/




    std::unique_ptr<Subwindow_SceneRenameModal> Subwindow_SceneRenameModal_; /**<Scene Rename Modal Instance*/
    std::unique_ptr<Subwindow_ModelRenameModal> Subwindow_ModelRenameModal_; /**<Model Rename Modal Instance*/
    std::unique_ptr<Subwindow_PointLightRenameModal> Subwindow_PointLightRenameModal_; /**<PointLight Rename Modal Instance*/
    std::unique_ptr<Subwindow_DirectionalLightRenameModal> Subwindow_DirectionalLightRenameModal_; /**<DirectionalLight Rename Modal Instance*/
    std::unique_ptr<Subwindow_SpotLightRenameModal> Subwindow_SpotLightRenameModal_; /**<SpotLight Rename Modal Instance*/
    
    std::unique_ptr<Subwindow_ModelReplaceModal> Subwindow_ModelReplaceModal_; /**<Window with functions to replace all existing instances of a model with another one*/

    std::unique_ptr<Subwindow_DeleteScene> Subwindow_DeleteScene_; /**<Scene Deletion Modal Instance*/
    std::unique_ptr<Subwindow_DeleteModel> Subwindow_DeleteModel_; /**<Model Deletion Modal Instance*/
    std::unique_ptr<Subwindow_DeletePointLight> Subwindow_DeletePointLight_; /**<PointLight Delete Subwindow*/
    std::unique_ptr<Subwindow_DeleteDirectionalLight> Subwindow_DeleteDirectionalLight_; /**<DirectionalLight Delete Subwindow*/
    std::unique_ptr<Subwindow_DeleteSpotLight> Subwindow_DeleteSpotLight_; /**<SpotLight Delete Subwindow*/
    



private:

    /**
     * @brief Draw A Scene Under A Tree Node
     * 
     * @param Scene 
     * @param SceneIndex
     */
    void DrawScene(ERS_STRUCT_Scene* Scene, int SceneIndex);




public:

    // Show/Hide Var
    bool Enabled_ = true; /**<Show/Hide Window*/

public:

    /**
     * @brief Construct a new Window_SceneTree object
     * 
     */
    GUI_Window_SceneTree(ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, Cursors3D* Cursors3D);

    /**
     * @brief Destroy the Window_SceneTree object
     * 
     */
    ~GUI_Window_SceneTree();

    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};