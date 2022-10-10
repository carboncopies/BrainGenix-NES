//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief This struct stores the opengl ids for a full depth map 'object'. 
 * Ie: the FBO id, Texture ID, etc.
 * 
 * Please Note: The depth maps are managed by 'ERS_CLASS_DepthMaps' not the light itself.
 * The depth maps are created and rendered-to by that class.
 * 
 */
struct ERS_STRUCT_DepthMap {

    unsigned int FrameBufferObjectID; /**<OpenGL Depth Map FBO IDs*/

    int DepthMapTextureIndex; /**<OpenGL Depth Map Texture Index In 2D Arrays*/
    glm::mat4 TransformationMatrix; /**<Used to store the transformation matrix*/

    bool Initialized = false; /**<Indiciates If The Target Is Initialized Or Not*/
    bool ToBeUpdated = true; /**<Indicated If This Depth Map Should Be Updated Next Frame*/

};