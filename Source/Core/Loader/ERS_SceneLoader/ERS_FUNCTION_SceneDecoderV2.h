//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Scene.h>
#include <ERS_STRUCT_Model.h>
#include <ERS_STRUCT_SceneCamera.h>
#include <ERS_STRUCT_SpotLight.h>
#include <ERS_STRUCT_PointLight.h>
#include <ERS_STRUCT_DirectionalLight.h>

#include <ERS_CLASS_ModelLoader.h>

#include <ERS_FUNCTION_YAMLHelpers.h>




/**
 * @brief Decodes the specified version of the scene format.
 * 
 * @param SceneData YAML::Node containing the scene data.
 * @param Scene Pointer to the scene to be updated.
 * @param SystemUtils Pointer to the systemutils struct.
 * @param LogEnable Enable or disable information log messages.
 * @return true Loading completed without errors.
 * @return false Loading failed.
 */
bool ERS_FUNCTION_DecodeSceneV2(YAML::Node SceneData, ERS_STRUCT_Scene *Scene, ERS_STRUCT_SystemUtils *SystemUtils, ERS_CLASS_ModelLoader* ModelLoader, bool LogEnable = true);