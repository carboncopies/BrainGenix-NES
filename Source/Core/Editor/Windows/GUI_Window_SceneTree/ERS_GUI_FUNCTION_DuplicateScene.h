 
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_Scene.h>

/**
 * @brief Duplicate the scene in the project
 * 
 * @param SceneManager 
 * @param SceneIndex 
 */
void GUI_Windowutil_DuplicateScene(ERS_CLASS_SceneManager* SceneManager, int SceneIndex);