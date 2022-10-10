 
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_PointLight.h>

/**
 * @brief Duplicate the PointLight
 * 
 * @param SceneManager 
 * @param SceneIndex 
 * @param PointLightIndex 
 */
void GUI_Windowutil_DuplicatePointLight(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int PointLightIndex);