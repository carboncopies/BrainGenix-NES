//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_ShaderUniformData.h>
#include <ERS_STRUCT_Shader.h>


// add function to set shader uniform data given shader data struct
// then update visualrenenderer to update that struct and pass it to the mesh renderer
// then update mesh renderer to only call update function when the model's requested shader is not the same as the current shader
// test 

/**
 * @brief Sets the target shader's uniform values to match with the data passed in.'
 * 
 * @param Shader 
 * @param Data 
 */
void ERS_FUNCTION_SetShaderUniformData(ERS_STRUCT_Shader* Shader, ERS_STRUCT_ShaderUniformData Data);