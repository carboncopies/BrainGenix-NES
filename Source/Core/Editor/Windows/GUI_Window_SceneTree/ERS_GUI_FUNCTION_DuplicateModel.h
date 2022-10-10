 
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_SceneManager.h>
#include <ERS_STRUCT_Model.h>

/**
 * @brief Duplicate the model
 * 
 * @param SceneManager 
 * @param SceneIndex 
 * @param ModelIndex 
 */
void GUI_Windowutil_DuplicateModel(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int ModelIndex);