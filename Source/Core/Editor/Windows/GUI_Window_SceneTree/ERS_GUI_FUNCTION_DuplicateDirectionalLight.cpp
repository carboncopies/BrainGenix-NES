//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_FUNCTION_DuplicateDirectionalLight.h>


void GUI_Windowutil_DuplicateDirectionalLight(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int DirectionalLightIndex) {

    // Get Current DirectionalLight
    ERS_STRUCT_DirectionalLight NewDirectionalLight = *SceneManager->Scenes_[SceneIndex]->DirectionalLights[DirectionalLightIndex];

    // Update Name
    std::string CurrentName = NewDirectionalLight.UserDefinedName;
    std::string NewName = CurrentName + std::string(" - Copy");
    NewDirectionalLight.UserDefinedName = NewName;

    NewDirectionalLight.DepthMap.Initialized = false;


    // Add To SceneManager
    SceneManager->Scenes_[SceneIndex]->DirectionalLights.push_back(std::make_shared<ERS_STRUCT_DirectionalLight>(NewDirectionalLight));

}