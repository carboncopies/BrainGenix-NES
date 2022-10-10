 
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_DirectionalLight.h>

/**
 * @brief Duplicate the DirectionalLight
 * 
 * @param SceneManager 
 * @param SceneIndex 
 * @param DirectionalLightIndex 
 */
void GUI_Windowutil_DuplicateDirectionalLight(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int DirectionalLightIndex);