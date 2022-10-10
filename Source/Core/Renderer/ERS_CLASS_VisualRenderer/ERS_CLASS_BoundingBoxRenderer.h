//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_Camera.h>
#include <ERS_STRUCT_Scene.h>




class ERS_CLASS_BoundingBoxRenderer {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_                   = nullptr;     /**<Shared Pointer To System Utils*/
    ERS_STRUCT_Shader*      BoundingBoxRendererShader_     = nullptr;     /**<Pointer To Shader With BoundingBoxRenderer*/
    glm::mat4               BoundingBoxRendererModelArray_;               /**<BoundingBoxRenderer Model Mat4*/

    unsigned int BoundingBoxRendererVBO_   = 0;    /**<OpenGL ID To BoundingBoxRenderer Vertex Buffer Object*/
    unsigned int BoundingBoxRendererVAO_   = 0;    /**<OpenGL ID To BoundingBoxRenderer Vertex Array Object*/
    float        BoundingBoxRendererScale_ = 0.1f; /**<Scale of editor icons*/
    int          BoundingBoxDisplayMode_   = 1;    /**<Sets the type of info displayed. 0 is a solid color, 1 is RAM priority, 2, VRAM priority, 3 is ram current texture level, 4 is vram current tex level*/ 
    bool         DisableDepthTest_         = true; /**<Tell the renderer to disable the depth test prior to drawing the boxes*/
    bool         DrawWireframe_            = true; /**<Tell the system to enable or disable wireframe mode*/

public:

    /**
     * @brief Construct a new ers class grid object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_BoundingBoxRenderer(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Shader* BoundingBoxRendererShader);

    /**
     * @brief Destroy the ers class grid object
     * 
     */
    ~ERS_CLASS_BoundingBoxRenderer();


    /**
     * @brief Renders the bounding boxes in the target scene from the camera's perspective
     * 
     */
    void DrawAll(ERS_STRUCT_Camera* Camera, ERS_STRUCT_Scene* Scene);

    /**
     * @brief Draw a bounding box around the target with the specified color.
     * 
     * @param Camera 
     * @param Model 
     * @param Color
     */
    void DrawModel(ERS_STRUCT_Camera* Camera, ERS_STRUCT_Model* Model, glm::vec3 Color = glm::vec3(0.4f, 0.4f, 0.0f));

    /**
     * @brief Enable or disable depth testing
     * 
     * @param DepthTestEnabled 
     */
    void SetDepthTest(bool DepthTestEnabled);

    /**
     * @brief Set the draw mode to be wireframe or solid
     * 
     * @param WireframeEnabled 
     */
    void SetDrawMode(bool WireframeEnabled);

    /**
     * @brief Set what kind of info the bounding boxes will show.
     * 0 means just a solid color
     * 1 Shows the Target RAM Level
     * 2 Shows the Target VRAM Level
     * 3 Shows Current RAM Level
     * 4 Shows Current VRAM Level
     * 
     * @param Mode 
     */
    void SetBoundingBoxDisplayMode(int Mode);

    /**
     * @brief Returns integer code of the bounding box display mode.
     * See SetBoundingBoxDisplayMode docs for more info about code meanings.
     * 
     * @return int 
     */
    int GetBoundingBoxDisplayMode();

};