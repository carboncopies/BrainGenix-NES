//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>

#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_Model.h>


/**
 * @brief Sorts the meshes in the given scene into the opaque and transparent meshes vectors. Also updates the "model" mat4 in the mesh.
 * 
 * @param OpaqueMeshes 
 * @param TransparentMeshes 
 * @param Scene 
 */
void ERS_FUNCTION_MeshTransparencySortNoTextures(std::vector<ERS_STRUCT_Mesh*>* OpaqueMeshes, std::vector<ERS_STRUCT_Mesh*>* TransparentMeshes, ERS_STRUCT_Scene* Scene);