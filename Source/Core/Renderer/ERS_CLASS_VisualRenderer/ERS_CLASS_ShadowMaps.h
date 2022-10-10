//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <map>
#include <algorithm>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_CLASS_DepthMaps.h>
#include <ERS_CLASS_MeshRenderer.h>

#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_DepthMap.h>
#include <ERS_STRUCT_Shader.h>

#include <ERS_ENUM_ShadowFilteringType.h>
#include <ERS_ENUM_ShadowUpdateMode.h>


/**
 * @brief This class renders any ui/overlay info onto the viewport as requested by the viewport struct.
 * 
 */
class ERS_CLASS_ShadowMaps {

private:

    ERS_STRUCT_SystemUtils*                 SystemUtils_           = nullptr; /**<Pointer to systemtuils resource*/
    ERS_STRUCT_ProjectUtils*                ProjectUtils_          = nullptr; /**<Pointer to project utils instance*/
    ERS_CLASS_MeshRenderer*                 Renderer_              = nullptr; /**<Pointer to mesh renderer instance*/
    std::mt19937                            MersenneTwister_;                 /**<Used to generate rnd numbers for random light updates*/
    std::uniform_int_distribution<uint32_t> RandomNumberGenerator_;           /**<Actual class that spits out random numbers*/

    uint32_t     RandomSeed_      = 1; /**Seed used to set initial random generator*/
    unsigned int LastUpdateIndex_ = 0; /**<Used when updating the depth maps, indicates when the last consecuitive index was*/


    /**
     * @brief Collect the depth maps from the scene's lights and create a list of them for the update system.
     * 
     * @param DepthMaps 
     * @param LightPositions 
     */
    void GetDepthMaps(std::vector<ERS_STRUCT_DepthMap*>* DepthMaps, std::vector<glm::vec3>* LightPositions);

    /**
     * @brief Determines which depth maps should be updated, and also deallocates those on lights that are no longer being used.
     * 
     * @param DepthMaps 
     * @param LightPositions 
     */
    void PrioritizeDepthMaps(std::vector<ERS_STRUCT_DepthMap*> DepthMaps, std::vector<glm::vec3> LightPositions, glm::vec3 CameraPosition);


public:

    std::unique_ptr<ERS_CLASS_DepthMaps> ERS_CLASS_DepthMaps_; /**<Instance of the depth map class*/



    ERS_STRUCT_DepthMap DeleteMe;


    /**
     * @brief Construct a new ers class ShadowMaps object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ShadowMaps(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_MeshRenderer* Renderer);

    /**
     * @brief Destroy the ers class ShadowMaps object
     * 
     */
    ~ERS_CLASS_ShadowMaps();



    /**
     * @brief This function will update the shadow maps of all lights in the scene.
     * This function checks the scenemanger and other shared classes to get info about the scene.
     * 
     */
    void UpdateShadowMaps(ERS_STRUCT_Shader* DepthMapShader, ERS_STRUCT_Shader* CubemapDepthShader, glm::vec3 CameraPosition);


    /**
     * @brief Deallocates all 
     * 
     */
    void DeallocateLightMaps();


};