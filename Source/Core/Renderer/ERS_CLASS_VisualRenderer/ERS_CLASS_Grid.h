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



class ERS_CLASS_Grid {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_    = nullptr;     /**<Shared Pointer To System Utils*/
    ERS_STRUCT_Shader*      GridShader_     = nullptr;     /**<Pointer To Shader With Grid*/
    glm::mat4               GridModelArray_;               /**<Grid Model Mat4*/

    unsigned int GridVBO_   = 0;     /**<OpenGL ID To Grid Vertex Buffer Object*/
    unsigned int GridVAO_   = 0;     /**<OpenGL ID To Grid Vertex Array Object*/
    float        GridScale_ = 25.0f; /**<Scale of the grid plane, does NOT set grid line scale. Use Grid Size For This Purpose.*/

public:

    float GridSize_ = 0.005f;
    float GridLineThickness_ = 0.01f;
    glm::vec3 GridColor_ = glm::vec3(0.75f); 
    glm::vec3 GridColorX_ = glm::vec3(1.0f, 0.0f, 0.0f); 
    glm::vec3 GridColorZ_ = glm::vec3(0.0f, 1.0f, 0.0f); 

public:

    /**
     * @brief Construct a new ers class grid object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_Grid(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Shader* GridShader);

    /**
     * @brief Destroy the ers class grid object
     * 
     */
    ~ERS_CLASS_Grid();

    
    /**
     * @brief Draw The Grid To The Viewport
     * 
     */
    void DrawGrid(glm::mat4 View, glm::mat4 Projection, glm::vec3 CameraPosition);

};