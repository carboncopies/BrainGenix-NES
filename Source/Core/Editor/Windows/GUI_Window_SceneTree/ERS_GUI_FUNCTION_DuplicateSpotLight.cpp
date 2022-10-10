//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_FUNCTION_DuplicateSpotLight.h>


void GUI_Windowutil_DuplicateSpotLight(ERS_CLASS_SceneManager* SceneManager, int SceneIndex, int SpotLightIndex) {

    // Get Current SpotLight
    ERS_STRUCT_SpotLight NewSpotLight = *SceneManager->Scenes_[SceneIndex]->SpotLights[SpotLightIndex];

    // Update Name
    std::string CurrentName = NewSpotLight.UserDefinedName;
    std::string NewName = CurrentName + std::string(" - Copy");
    NewSpotLight.UserDefinedName = NewName;

    NewSpotLight.DepthMap.Initialized = false;


    // Add To SceneManager
    SceneManager->Scenes_[SceneIndex]->SpotLights.push_back(std::make_shared<ERS_STRUCT_SpotLight>(NewSpotLight));

}