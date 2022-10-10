//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Internal Libraries (BG convention: use <> instead of "")



/**
 * @brief Camera object used in the game. Provides control for the user to move the game's camera as they please.
 * 
 */
struct ERS_STRUCT_SceneCamera {

    // Camea Configuration Information
    float NearClip_          = 0.01f;  /**<Closest distance before geometry is culled.*/
    float FarClip_           = 100.0f; /**<Farthest distance before geometry is called*/
    std::vector<long> AttachedScriptIndexes_; /**<Indices of scripts that are attached to this camera*/
    std::string UserDefinedName_; /**<Name that appears in the editor's scene tree*/

    // Internal Camera State Information
    float FOV_               = 70.0f;  /**<Field of view in degrees*/
    bool EnforceAspectRatio_ = false;  /**<Manually Override the aspect ratio with a set one*/
    float AspectRatio_       = 1.25f;  /**<Internal variable used to setup the projection matrix*/
    
    glm::vec3 Pos_; /**<Position of the camera object*/
    glm::vec3 Rot_; /**<Rotation of the camera object*/

    // Asset Streaming Settings
    int StreamingPriority_ = 1;                 /**< Higher this is, the more the system will try and load assets for this camera. Should be in range (1-10)*/

};