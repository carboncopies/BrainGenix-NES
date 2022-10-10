// ToDO: then add to project struct, then update project loader/writer with this info. Then check trello board for other related tasks like live ediitng.


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Struct for storing shader uniform data
 * 
 */
struct ERS_STRUCT_ShaderUniformDataDirectionalLight {

    glm::vec3 Direction_;
    glm::vec3 Color_;
    float Intensity_;
    float MaxDistance_;
    bool CastsShadows_;
    int DepthMapIndex_;
    glm::mat4 LightSpaceMatrix_;

};


/**
 * @brief Struct for storing shader uniform data
 * 
 */
struct ERS_STRUCT_ShaderUniformDataPointLight {

    glm::vec3 Position_;
    glm::vec3 Color_;
    float Intensity_;
    float MaxDistance_;
    bool CastsShadows_;
    int DepthCubemapIndex_;

};


/**
 * @brief Struct for storing shader uniform data
 * 
 */
struct ERS_STRUCT_ShaderUniformDataSpotLight {

    glm::vec3 Position_;
    glm::vec3 Direction_;
    glm::vec3 Color_;
    float Intensity_;
    float CutOff_;
    float RollOff_;

    float MaxDistance_;
    bool CastsShadows_;
    int DepthMapIndex_;
    glm::mat4 LightSpaceMatrix_;

};


/**
 * @brief Struct for storing shader uniform data
 * 
 */
struct ERS_STRUCT_ShaderUniformData {

    // Shader Metadata
    float Time_;
    float FrameTime_;
    int FrameNumber_;
    glm::vec2 ViewportRes_;
    glm::vec3 CameraPosition_;
    float ShininessOffset_;
    
    // Viewport Controls
    bool GammaCorrectionEnabled_;
    bool HDREnabled_;
    float Exposure_;
    float Gamma_;

    // Camera Info
    glm::mat4 Projection_;
    glm::mat4 View_;


    // Shadow Info
    int ShadowFilterType_;
    int ShadowFilterKernelSize_;
    unsigned int DepthMapArray_;
    unsigned int DepthCubemapArray_;


    // Lighting Info
    int NumberDirectionalLights_;
    std::vector<ERS_STRUCT_ShaderUniformDataDirectionalLight> DirectionalLights_;
    int NumberPointLights_;
    std::vector<ERS_STRUCT_ShaderUniformDataPointLight> PointLights_;
    int NumberSpotLights_;
    std::vector<ERS_STRUCT_ShaderUniformDataSpotLight> SpotLights_;

};