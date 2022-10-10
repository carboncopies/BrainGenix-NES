//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_CLASS_MeshRenderer.h>

#include <ERS_SceneManager.h>

#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Viewport.h>
#include <ERS_STRUCT_DepthMap.h>



#include <ERS_FUNCTION_ConvertRotationToFrontVector.h>


struct ERS_STRUCT_CubemapFBOIndexes {

    long FBO1 = -1;
    long FBO2 = -1;
    long FBO3 = -1;
    long FBO4 = -1;
    long FBO5 = -1;
    long FBO6 = -1;
    
};




/**
 * @brief This class renders any ui/overlay info onto the viewport as requested by the viewport struct.
 * 
 */
class ERS_CLASS_DepthMaps {

private:

    ERS_STRUCT_SystemUtils*  SystemUtils_  = nullptr; /**<Pointer to systemtuils resource*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_ = nullptr; /**<Pointer to project utils instance*/
    ERS_CLASS_MeshRenderer*  Renderer_     = nullptr; /**<Pointer to Mesh Renderer Class Instance*/

    unsigned int DepthMapAllocationChunkSize_ = 16; /**<Allocate Depth Map In Chunks Of 5*/








public:

    unsigned int DepthTextureArrayID_; /**<OpenGL Object ID For Depth Map Textures*/
    unsigned int DepthTextureCubemapArrayID_; /**<OpenGL Object ID For Depth Texture Cubemap Array*/
    int DepthTextureArrayWidth_ = 2048; /***<Width of the depth map texture*/
    int DepthTextureArrayHeight_ = 2048; /**<Height of the depth map texutre*/
    int DepthTextureNumTextures_ = 16; /**<Number of textures that the tex array can hold*/
    int DepthTextureCubemapNumTextures_ = 2; /**<Number of cubemaps the array can hold*/
    std::vector<long> DepthMapTexturesAlreadyAllocated_; /**<Use This To Check if the texture is already allocated or not*/
    //std::vector<ERS_STRUCT_CubemapFBOIndexes> DepthMapTexturesCubemapAlreadyAllocated_; /**<Used to check if a given cubemap index is in use, and to indiciate it's framebuffer objects, so they can be rebound on resize*/
    std::vector<bool> DepthMapTexturesCubemapAlreadyAllocated_; /**<Used to check if a given cubemap index is in use, and to indiciate it's framebuffer objects, so they can be rebound on resize*/
    

    unsigned int CubemapFBO_;
    unsigned int PointLightClearFBO_;

    // !!FIXME!!
    // Replace above vector with instead pointers to struct list so we can keep track and automatically free, etc.

    //std::vector<unsigned int> DepthMapTextureIndex_; /**<Use This To Check if the texture is already allocated or not*/
    //std::vector<unsigned int> AssociatedDepthMapFramebufferIndex_; /**<Used To Pair The Framebuffer With The Depth Map*/


    /**
     * @brief Construct a new ers class DepthMaps object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_DepthMaps(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_MeshRenderer* Renderer);

    /**
     * @brief Destroy the ers class DepthMaps object
     * 
     */
    ~ERS_CLASS_DepthMaps();


    /**
     * @brief Checks the settings of the renderer against the current parameters.
     * 
     */
    void CheckSettings();


    /**
     * @brief Deallocates an index from the depth map texture array.
     * @param Index
     * @return true 
     * @return false 
     */
    bool FreeDepthMapIndex2D(unsigned int Index);
    bool FreeDepthMapIndexCubemap(unsigned int Index);
    

    /**
     * @brief Allocate an index from the depth map texture array.
     * 
     * @return unsigned int 
     */
    unsigned int AllocateDepthMapIndex2D(unsigned int FramebufferObjectID);
    unsigned int AllocateDepthMapIndexCubemap();


    /**
     * @brief Generates a depth map array (array texture), with the given width, height and number of layers.
     * 
     * @param NumberOfTextures Number of textures in the array
     * @param Width Height of the array
     * @param Height Width of the array
     * @param LogEnabled Enab;e/Disable Log Output
     * @return true 
     * @return false 
     */
    bool RegenerateDepthMapTextureArray2D(int NumberOfTextures, int Width = 2048, int Height = 2048, bool LogEnabled = true);
    bool RegenerateDepthMapTextureArrayCubemap(int NumberOfTextures, bool LogEnabled = true);

    /**
     * @brief Generates a depth map with the given resolution.
     * This is usually used for shadows later on in the rendering process.
     * Resolution is specified by the 
     * 
     * @param Number The Number OF Depth Maps To Assign To The Light In Question
     * @param LogOutput Enable/disable logging output
     * @return ERS_STRUCT_DepthMap Struct containing the relevant opengl ids for this depth map
     */
    ERS_STRUCT_DepthMap GenerateDepthMap2D(int Number = 1, bool LogOutput = true);
    //ERS_STRUCT_DepthMap GenerateDepthMapCubemap(bool LogOutput = true);

    /**
     * @brief Checks the number of depth maps vs the number of lights and makes sure that the number of depth maps is equal to the number of lights.
     * Will generate more depth maps and remove unused ones by calling internal functions in this class.
     * 
     */
    void UpdateNumberOfDepthMaps2D();

    /**
     * @brief Iterates over all lights in the scene and renders all depth maps.
     * 
     * @param DepthShader 
     */
    void UpdateDepthMaps(ERS_STRUCT_Shader* DepthShader, ERS_STRUCT_Shader* CubemapDepthShader);

    /**
     * @brief This function creates a depth map from the perspective of the given position data.
     * The depth map texture is cleared, and then rendered to.
     * This function is designed to be used in shadow maps, but can be used elsewhere when appropriate.
     * 
     * @param Target 
     * @param Pos 
     * @param Rot 
     * @param Scale 
     * @param Orthogonal Use Orthogonal for directional lights and perspective for everything else
     */
    void UpdateDepthMap(ERS_STRUCT_DirectionalLight* Light, ERS_STRUCT_Shader* DepthShader);
    void UpdateDepthMap(ERS_STRUCT_PointLight* Light, ERS_STRUCT_Shader* DepthShader);
    void UpdateDepthMap(ERS_STRUCT_SpotLight* Light, ERS_STRUCT_Shader* DepthShader);

};