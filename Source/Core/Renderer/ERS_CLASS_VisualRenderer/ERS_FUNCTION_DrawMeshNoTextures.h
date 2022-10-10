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

#include <ERS_FUNCTION_ResetMeshTexture.h>

#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_Model.h>



/**
 * @brief Draws the mesh passed in.
 * 
 * @param Mesh 
 */
void ERS_FUNCTION_DrawMeshNoTextures(ERS_STRUCT_Mesh* Mesh, ERS_STRUCT_Shader* Shader);