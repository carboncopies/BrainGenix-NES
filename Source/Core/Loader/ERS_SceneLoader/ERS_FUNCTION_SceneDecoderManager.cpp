//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_SceneDecoderManager.h>




bool ERS_FUNCTION_DecodeScene(YAML::Node SceneData, ERS_STRUCT_Scene *Scene, ERS_STRUCT_SystemUtils *SystemUtils, ERS_CLASS_ModelLoader* ModelLoader, bool LogEnable) {

    SystemUtils->Logger_->Log("Decoding ERS Scene", 5, LogEnable);
    
    // Extract Current Version
    int Version = 0;
    SystemUtils->Logger_->Log("Identifying Scene Format Version", 4, LogEnable);
    if (SceneData["SceneFormatVersion"]) {
        Version = SceneData["SceneFormatVersion"].as<int>();
    } else {
        SystemUtils->Logger_->Log("Unable To Find Metadata Tag For 'SceneFormatVersion', Unable To Identify Version", 9);
        return false;
    }

    // Attempt Decoding With Known Decoding Systems
    if (Version == 1) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 1, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV1(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);
    } else if (Version == 2) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 2, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV2(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);        
    } else if (Version == 3) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 3, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV3(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);
    } else if (Version == 4) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 4, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV4(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);
    } else if (Version == 5) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 5, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV5(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);
    } else if (Version == 6) {
        SystemUtils->Logger_->Log("Detected Scene Format To Be Version 6, Starting Decoding Process", 4, LogEnable);
        return ERS_FUNCTION_DecodeSceneV6(SceneData, Scene, SystemUtils, ModelLoader, LogEnable);
    } else {
        SystemUtils->Logger_->Log("Scene Format Has Unknown Version, Aborting Load", 8);
        return false;
    }

}