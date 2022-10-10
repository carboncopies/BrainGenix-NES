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
#include <ERS_STRUCT_OpenGLDefaults.h>
#include <ERS_STRUCT_Camera.h>
#include <ERS_STRUCT_Scene.h>

#include <ERS_SceneManager.h>



class ERS_CLASS_IconRenderer {

private:

    ERS_STRUCT_SystemUtils*    SystemUtils_            = nullptr; /**<Shared Pointer To System Utils*/
    ERS_STRUCT_OpenGLDefaults* OpenGLDefaults_         = nullptr; /**<Pointer acquired from renderermanager*/
    ERS_STRUCT_Shader*         IconRendererShader_     = nullptr; /**<Pointer To Shader With IconRenderer*/
    glm::mat4                  IconRendererModelArray_;           /**<IconRenderer Model Mat4*/

    unsigned int IconRendererVBO_   = 0;    /**<OpenGL ID To IconRenderer Vertex Buffer Object*/
    unsigned int IconRendererVAO_   = 0;    /**<OpenGL ID To IconRenderer Vertex Array Object*/
    float        IconRendererScale_ = 0.1f; /**<Scale of editor icons*/


public:

    /**
     * @brief Construct a new ers class grid object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_IconRenderer(ERS_STRUCT_OpenGLDefaults* Defaults, ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Shader* IconRendererShader);

    /**
     * @brief Destroy the ers class grid object
     * 
     */
    ~ERS_CLASS_IconRenderer();


    /**
     * @brief Draw Command
     * 
     */
    void Draw(ERS_STRUCT_Camera* Camera, ERS_CLASS_SceneManager* SceneManager);

};