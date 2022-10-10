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
 * @brief Binds to the given texture using the given shader program.
 * 
 * @param Name 
 * @param Offset 
 * @param ShaderProgram 
 * @param TextureID 
 */
void ERS_FUNCTION_ResetMeshTexture(const char* Name, int Offset, unsigned int ShaderProgram, unsigned int TextureID);