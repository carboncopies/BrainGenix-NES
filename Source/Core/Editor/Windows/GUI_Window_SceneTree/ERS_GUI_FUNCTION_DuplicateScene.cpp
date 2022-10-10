//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_FUNCTION_DuplicateScene.h>


void GUI_Windowutil_DuplicateScene(ERS_CLASS_SceneManager* SceneManager, int SceneIndex) {

    // Get Current Scene
    ERS_STRUCT_Scene NewScene = *SceneManager->Scenes_[SceneIndex].get();

    // Update Name
    std::string CurrentName = NewScene.SceneName;
    std::string NewName = CurrentName + std::string(" - Copy");
    NewScene.SceneName = NewName;

    // Add To SceneManager
    SceneManager->Scenes_.push_back(std::make_unique<ERS_STRUCT_Scene>(NewScene));

}