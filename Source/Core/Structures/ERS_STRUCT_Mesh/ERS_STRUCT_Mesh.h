//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vector>
#include <string>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_Texture.h>
#include <ERS_STRUCT_Vertex.h>
#include <ERS_STRUCT_OpenGLDefaults.h>




/**
 * @brief This struct contains a mesh object that is a part of the model struct. It is ment for ERS Internal use.
 * 
 */
struct ERS_STRUCT_Mesh {

    // OpenGL IDs
    unsigned int VBO; /**<Vertex buffer object OpenGL ID.*/
    unsigned int EBO; /**<OpenGL EBO Object ID*/
    unsigned int VAO; /**<Vertex Array Object OpenGL Handle*/

    // Loading System
    std::vector<std::pair<std::string, std::string>> Loader_RequestedTextureInformation_; /**<Contains the file path for the original texture paired with the texture's type as identified by assimp.*/

    std::vector<ERS_STRUCT_Texture*> Textures_; /**<Pointers to the textures for this mesh - the Model struct owns these*/

    std::vector<ERS_STRUCT_Vertex> Vertices; /**<Array of Mesh Vertices*/
    std::vector<unsigned int> Indices; /**<Array of Mesh Vertex Indices*/
    long NumberIndices; /**<Size of Indices Array*/

    std::vector<int> TextureColorChannels_; /**<List Of Number Of Color Channels In Texture*/

    bool HasTransparency_; /**<Indicates if this model has any transparent textures (detected by number of channels)*/
    glm::mat4 ModelMatrix; /**<Model Matrix Given To The Shader When Rendering*/
    
    // Renderer Settings
    int ShaderOverrideIndex_ = -1; /**<Updated by the renderer - determines if we need to overried the default viewport shader*/  

    bool* CastDynamicShadows_; /**<Indicate if this model is to be rendered when generating the depth maps (dynamically)*/
    bool* CastStaticShadows_; /**<Set if this model is rendered when building static depth maps*/
    bool* ReceiveShadows_ ; /**<Determines if this model is to have shadows applied to it.*/

    bool* TreatMissingTexturesAsTransparent_ = nullptr; /**<Treats missing textures as transparent.*/ 


    /**
     * @brief Setup the mesh (create opengl objects, etc.)
     * 
     */
    void SetupMesh();


    /**
     * @brief Reset the texture to the default with the given name and offset.
     * 
     * @param Name 
     * @param Offset 
     */
    void ResetTexture(const char* Name, int Offset, unsigned int ShaderProgram, unsigned int TextureID);

    /**
     * @brief Updates if the model is transparent or not
     * 
     */
    void UpdateTransparencyStatus(); 


};
