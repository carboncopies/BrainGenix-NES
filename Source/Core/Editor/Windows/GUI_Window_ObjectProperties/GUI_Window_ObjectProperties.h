//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_Editor_3DCursor.h>

#include <ERS_STRUCT_LocRotScale.h>
#include <ERS_STRUCT_ProjectUtils.h>

#include <ERS_CLASS_VisualRenderer.h>

#include <ERS_SceneManager.h>



/**
 * @brief Create GUI GUI_Window "Rendering Settings"
 * 
 */
class GUI_Window_ObjectProperties {

private:

    Cursors3D* Cursors3D_; /**<Instance pointer to cursors3d class*/
    ERS_CLASS_SceneManager* SceneManager_; /**<Pointer To Scene Manager Instance*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Pointer to projectutils struct*/
    ERS_CLASS_VisualRenderer* VisualRenderer_; /**<Pointer to Visualrenderer used to get shader info*/

    std::vector<long>* ScriptIndices_; /**<variable to store script names for list box*/
    //char ShaderNames_[32][512]; /**<List of names to be used to store shader's names*/
    int ScriptIndex_; /**<Selected index of script*/


    /**
     * @brief Converts From A Vec To A Float*
     * 
     * @param In 
     * @param Out 
     */
    void VecToFloat(glm::vec3* In, float* Out);

    /**
     * @brief Converts from A Float* To A Vec
     * 
     * @param In 
     * @param Out 
     */
    void FloatToVec(float* In, glm::vec3* Out);

    /**
     * @brief Draws a color coded drag float element.
     * 
     * @param Name 
     * @param Value 
     * @param SnapAmount
     * @return glm::vec3 
     */
    glm::vec3 XYZDragFloat(std::string Name, glm::vec3 Value, float SnapAmount = 0.1f);


public:

    // Show/Hide Var
    bool Enabled_ = true; /**<Show/Hide GUI_Window*/

    /**
     * @brief Construct a new GUI_Window_ObjectProperties object
     * 
     */
    GUI_Window_ObjectProperties(Cursors3D* Cursors3D, ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_VisualRenderer* VisualRenderer);

    /**
     * @brief Destroy the GUI_Window_ObjectProperties object
     * 
     */
    ~GUI_Window_ObjectProperties();

    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};