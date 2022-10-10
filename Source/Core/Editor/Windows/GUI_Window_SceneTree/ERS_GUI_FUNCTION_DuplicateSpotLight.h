 
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_SpotLight.h>

/**
 * @brief Duplicate the SpotLight
 * 
 * @param SceneManager 
 * @param SceneIndex 
 * @param SpotLightIndex 
 */
void GUI_Windowutil_DuplicateSpotLight(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int SpotLightIndex);