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
#include <ERS_STRUCT_Scene.h>


/**
 * @brief Updates the transparency bool in each mesh, allowing for sorting to take place
 * 
 * @param Scene 
 */
void ERS_FUNCTION_UpdateMeshTransparency(ERS_STRUCT_Scene* Scene);