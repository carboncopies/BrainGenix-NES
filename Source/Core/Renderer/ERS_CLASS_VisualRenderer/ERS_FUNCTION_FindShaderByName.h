//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>
#include <map>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Shader.h>


/**
 * @brief Returns the index of a shader with a name that matches the input name.
 * Will return 0 if the shader cannot be found.
 * 
 * @param Name 
 * @param Shaders 
 */
unsigned int ERS_FUNCTION_FindShaderByName(std::string Name, std::vector<std::unique_ptr<ERS_STRUCT_Shader>>* Shaders);