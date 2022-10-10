//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_SceneDecoderV2.h>


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_SceneDecoderV1.h>



bool ERS_FUNCTION_DecodeSceneV2(YAML::Node SceneData, ERS_STRUCT_Scene *Scene, ERS_STRUCT_SystemUtils *SystemUtils, ERS_CLASS_ModelLoader* ModelLoader, bool LogEnable) {

    // Init
    bool Success = true;
    SystemUtils->Logger_->Log(std::string("Processing Scene '") + Scene->SceneName + "' With Decoder Version 2", 3, LogEnable);
    ERS_LoggingSystem* Logger = SystemUtils->Logger_.get();

    // Grab Metadata
    std::vector<YAML::Node> SceneItems;
    Success &= ERS_FUNCTION_GetLong       (Logger, SceneData, "SceneFormatVersion", Scene->SceneFormatVersion      );
    Success &= ERS_FUNCTION_GetString     (Logger, SceneData, "SceneName",          Scene->SceneName               );
    Success &= ERS_FUNCTION_GetInt        (Logger, SceneData, "ActiveCameraIndex",  Scene->ActiveSceneCameraIndex  );
    Success &= ERS_FUNCTION_GetNodeVector (Logger, SceneData, "SceneData",          SceneItems                     );

    // Iterate Through Vector To Add Each Asset To Loading Queue Of Requested Type
    for (long i = 0; (long)i < (long)SceneItems.size(); i++) {

        // Get Asset Information
        YAML::Node Item = SceneItems[i];
        std::string AssetName, AssetType;
        Success &= ERS_FUNCTION_GetString(Logger, Item, "AssetName", AssetName);
        Success &= ERS_FUNCTION_GetString(Logger, Item, "AssetType", AssetType);


        if (AssetType == "Model") {

            ERS_STRUCT_Model Model;
            Success &= ERS_FUNCTION_GetLong       (Logger, Item, "AssetID",              Model.AssetID                 );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "AssetPosition",        Model.ModelPosition           );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "AssetRotation",        Model.ModelRotation           );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "AssetScale",           Model.ModelScale              );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "CastDynamicShadows",   Model.CastDynamicShadows_     );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "CastStaticShadows",    Model.CastStaticShadows_      );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "ReceiveShadows",       Model.ReceiveShadows_         );
            Success &= ERS_FUNCTION_GetLong       (Logger, Item, "ShaderOverrideIndex",  Model.ShaderOverrideIndex_    );
            Success &= ERS_FUNCTION_GetString     (Logger, Item, "AssetName",            Model.Name                    );
            Success &= ERS_FUNCTION_GetLongVector (Logger, Item, "AttachedScripts",      Model.AttachedScriptIndexes_  );

            // Adjust Model Scale Based On New Metric Scale Parameters
            Model.SetScale(Model.ModelScale * 100.0f);


            Scene->Models.push_back(std::make_shared<ERS_STRUCT_Model>(Model));
            ModelLoader->AddModelToLoadingQueue(Scene->Models[Scene->Models.size()-1]);
            Scene->Models[Scene->Models.size()-1]->ApplyTransformations();

        } else if (AssetType == std::string("DirectionalLight")) {

            ERS_STRUCT_DirectionalLight Light;
            Success &= ERS_FUNCTION_GetString     (Logger, Item, "AssetName",            Light.UserDefinedName         );
            Success &= ERS_FUNCTION_GetVec3Color  (Logger, Item, "Color",                Light.Color                   );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Pos",                  Light.Pos                     );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Rot",                  Light.Rot                     );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "Intensity",            Light.Intensity               );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "MaxDistance",          Light.MaxDistance             );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "CastShadows",          Light.CastsShadows_           );
            Success &= ERS_FUNCTION_GetLongVector (Logger, Item, "AttachedScripts",      Light.AttachedScriptIndexes_  );
            Scene->DirectionalLights.push_back(std::make_shared<ERS_STRUCT_DirectionalLight>(Light));

        } else if (AssetType == std::string("PointLight")) {

            ERS_STRUCT_PointLight Light;
            Success &= ERS_FUNCTION_GetString     (Logger, Item, "AssetName",            Light.UserDefinedName         );
            Success &= ERS_FUNCTION_GetVec3Color  (Logger, Item, "Color",                Light.Color                   );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Pos",                  Light.Pos                     );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "Intensity",            Light.Intensity               );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "MaxDistance",          Light.MaxDistance             );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "CastShadows",          Light.CastsShadows_           );
            Success &= ERS_FUNCTION_GetLongVector (Logger, Item, "AttachedScripts",      Light.AttachedScriptIndexes_  );
            Scene->PointLights.push_back(std::make_shared<ERS_STRUCT_PointLight>(Light));

        } else if (AssetType == std::string("SpotLight")) {

            ERS_STRUCT_SpotLight Light;
            Success &= ERS_FUNCTION_GetString     (Logger, Item, "AssetName",            Light.UserDefinedName         );
            Success &= ERS_FUNCTION_GetVec3Color  (Logger, Item, "Color",                Light.Color                   );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Pos",                  Light.Pos                     );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Rot",                  Light.Rot                     );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "Intensity",            Light.Intensity               );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "MaxDistance",          Light.MaxDistance             );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "CutOff",               Light.CutOff                  );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "RollOff",              Light.Rolloff                 );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "CastShadows",          Light.CastsShadows_           );
            Success &= ERS_FUNCTION_GetLongVector (Logger, Item, "AttachedScripts",      Light.AttachedScriptIndexes_  );
            Scene->SpotLights.push_back(std::make_shared<ERS_STRUCT_SpotLight>(Light));

        } else if (AssetType == std::string("SceneCamera")) {

            ERS_STRUCT_SceneCamera Camera;
            Success &= ERS_FUNCTION_GetString     (Logger, Item, "AssetName",            Camera.UserDefinedName_       );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Pos",                  Camera.Pos_                   );
            Success &= ERS_FUNCTION_GetVec3       (Logger, Item, "Rot",                  Camera.Rot_                   );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "NearClip",             Camera.NearClip_              );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "FarClip",              Camera.FarClip_               );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "FOV",                  Camera.FOV_                   );
            Success &= ERS_FUNCTION_GetBool       (Logger, Item, "EnforceAspectRatio",   Camera.EnforceAspectRatio_    );
            Success &= ERS_FUNCTION_GetFloat      (Logger, Item, "AspectRatio",          Camera.AspectRatio_           );
            Success &= ERS_FUNCTION_GetInt        (Logger, Item, "StreamingPriority",    Camera.StreamingPriority_     );
            Success &= ERS_FUNCTION_GetLongVector (Logger, Item, "AttachedScripts",      Camera.AttachedScriptIndexes_ );
            Scene->SceneCameras.push_back(std::make_shared<ERS_STRUCT_SceneCamera>(Camera));

        } else {
            SystemUtils->Logger_->Log(std::string("Unsupported/Unknown Asset Type: ") + AssetType, 9);
        }

    }

    // Indicate Scene Is Loaded
    if (!Success) {
        SystemUtils->Logger_->Log("Scene Decoding Failed", 8);
    } else {
        SystemUtils->Logger_->Log("Finished Decoding Scene", 4);
    }
    Scene->IsSceneLoaded = Success;
    return Success;
}