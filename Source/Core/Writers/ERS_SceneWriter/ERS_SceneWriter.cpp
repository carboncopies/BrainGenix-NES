//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

// FIXME: Make template textures added to their own map so they can be reused.
// FIXME: Make dynamic/static models that can each be rotated/translated (dynamically or statically). Then make updateposition functions, etc.

#include <ERS_SceneWriter.h>

SceneWriter::SceneWriter(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing SceneWriter Subsystem", 5);

}

SceneWriter::~SceneWriter() {

    SystemUtils_->Logger_->Log("SceneWriter Destructor Called", 6);

}

void SceneWriter::ProcessScene(ERS_STRUCT_Scene* InputScene, long AssetID) {

    // Convert Scene To YAML Encoded String
    std::string SceneByteString = ProcessScene(InputScene); 

    // Copy Into System IOData Struct
    std::unique_ptr<ERS_STRUCT_IOData> SceneData = std::make_unique<ERS_STRUCT_IOData>();

    SceneData->Data.reset(new unsigned char[SceneByteString.size()]);
    SceneData->Size_B = SceneByteString.size();
    memcpy(SceneData->Data.get(), SceneByteString.c_str(), SceneByteString.size());

    // Write To Storage
    SystemUtils_->ERS_IOSubsystem_->WriteAsset(AssetID, SceneData.get());

}

std::string SceneWriter::ProcessScene(ERS_STRUCT_Scene* InputScene) {

    // Log Scene Write
    SystemUtils_->Logger_->Log(std::string(std::string("Serializing Scene '") + InputScene->SceneName + std::string("'")).c_str(), 4);

    // Create Emitter
    YAML::Emitter Output;

    // Begin Writing
    Output << YAML::BeginMap;

    // Write Metadata
    Output << YAML::Key << "SceneName" << YAML::Value << InputScene->SceneName;
    Output << YAML::Key << "SceneFormatVersion" << YAML::Value << 6;
    Output << YAML::Key << "ActiveCameraIndex" << YAML::Value << InputScene->ActiveSceneCameraIndex;
    

    //---- Write Models ----//
    Output << YAML::Key << "Models";
    Output << YAML::Key << YAML::BeginMap;
    for (int i = 0; (long)i < (long)InputScene->Models.size(); i++) {


        Output << YAML::Key << i;
        Output << YAML::BeginMap;


        Output << YAML::Key << "AssetName" << YAML::Value << InputScene->Models[i]->Name;
        Output << YAML::Key << "AssetType" << YAML::Value << "Model";
        Output << YAML::Key << "AssetID" << YAML::Value << InputScene->Models[i]->AssetID;


        Output << YAML::Key << "AssetPositionX" << YAML::Value << InputScene->Models[i]->ModelPosition[0];
        Output << YAML::Key << "AssetPositionY" << YAML::Value << InputScene->Models[i]->ModelPosition[1];
        Output << YAML::Key << "AssetPositionZ" << YAML::Value << InputScene->Models[i]->ModelPosition[2];

        Output << YAML::Key << "AssetRotationX" << YAML::Value << InputScene->Models[i]->ModelRotation[0];
        Output << YAML::Key << "AssetRotationY" << YAML::Value << InputScene->Models[i]->ModelRotation[1];
        Output << YAML::Key << "AssetRotationZ" << YAML::Value << InputScene->Models[i]->ModelRotation[2];

        Output << YAML::Key << "AssetScaleX" << YAML::Value << InputScene->Models[i]->ModelScale[0];
        Output << YAML::Key << "AssetScaleY" << YAML::Value << InputScene->Models[i]->ModelScale[1];
        Output << YAML::Key << "AssetScaleZ" << YAML::Value << InputScene->Models[i]->ModelScale[2];

        Output << YAML::Key << "ModelMinLOD" << YAML::Value << InputScene->Models[i]->UserLimitedMinLOD_;
        Output << YAML::Key << "ModelMaxLOD" << YAML::Value << InputScene->Models[i]->UserLimitedMaxLOD_;
        

        Output << YAML::Key << "CastDynamicShadows" << YAML::Value << InputScene->Models[i]->CastDynamicShadows_;
        Output << YAML::Key << "CastStaticShadows" << YAML::Value << InputScene->Models[i]->CastStaticShadows_;
        Output << YAML::Key << "ReceiveShadows" << YAML::Value << InputScene->Models[i]->ReceiveShadows_;
        
        Output << YAML::Key << "TreatMissingTexturesAsTransparent" << YAML::Value << InputScene->Models[i]->TreatMissingTexturesAsTransparent_;

        Output << YAML::Key << "ShaderOverrideIndex" << YAML::Value << InputScene->Models[i]->ShaderOverrideIndex_;



        Output<<YAML::Key<<"AttachedScripts";
        Output<<YAML::Key<<YAML::BeginMap;
        for (unsigned long x = 0; x < InputScene->Models[i]->AttachedScriptIndexes_.size(); x++) {
            Output<<YAML::Key<<x<<YAML::Value<<InputScene->Models[i]->AttachedScriptIndexes_[x];
        }
        Output<<YAML::EndMap;


        Output << YAML::EndMap;
    }
    Output << YAML::EndMap;


    //---- Write Directional Lights ----//
    Output << YAML::Key << "DirectionalLights";
    Output << YAML::Key << YAML::BeginMap;
    for (int i = 0; (long)i < (long)InputScene->DirectionalLights.size(); i++) {

        Output << YAML::Key << i;
        Output << YAML::BeginMap;


        Output << YAML::Key << "AssetName" << YAML::Value << InputScene->DirectionalLights[i]->UserDefinedName;
        Output << YAML::Key << "AssetType" << YAML::Value << "DirectionalLight";


        Output << YAML::Key << "ColorRed" << YAML::Value << InputScene->DirectionalLights[i]->Color[0];
        Output << YAML::Key << "ColorGreen" << YAML::Value << InputScene->DirectionalLights[i]->Color[1];
        Output << YAML::Key << "ColorBlue" << YAML::Value << InputScene->DirectionalLights[i]->Color[2];

        Output << YAML::Key << "PosX" << YAML::Value << InputScene->DirectionalLights[i]->Pos[0];
        Output << YAML::Key << "PosY" << YAML::Value << InputScene->DirectionalLights[i]->Pos[1];
        Output << YAML::Key << "PosZ" << YAML::Value << InputScene->DirectionalLights[i]->Pos[2];

        Output << YAML::Key << "RotX" << YAML::Value << InputScene->DirectionalLights[i]->Rot[0];
        Output << YAML::Key << "RotY" << YAML::Value << InputScene->DirectionalLights[i]->Rot[1];
        Output << YAML::Key << "RotZ" << YAML::Value << InputScene->DirectionalLights[i]->Rot[2];

        Output << YAML::Key << "Intensity" << YAML::Value << InputScene->DirectionalLights[i]->Intensity;
        Output << YAML::Key << "MaxDistance" << YAML::Value << InputScene->DirectionalLights[i]->MaxDistance;

        Output << YAML::Key << "CastShadows" << YAML::Value << InputScene->DirectionalLights[i]->CastsShadows_;


        Output<<YAML::Key<<"AttachedScripts";
        Output<<YAML::Key<<YAML::BeginMap;
        for (unsigned long x = 0; x < InputScene->DirectionalLights[i]->AttachedScriptIndexes_.size(); x++) {
            Output<<YAML::Key<<x<<YAML::Value<<InputScene->DirectionalLights[i]->AttachedScriptIndexes_[x];
        }
        Output<<YAML::EndMap;

        Output << YAML::EndMap;
    }
    Output << YAML::EndMap;

    //---- Write Point Lights ----//
    Output << YAML::Key << "PointLights";
    Output << YAML::Key << YAML::BeginMap;
    for (int i = 0; (long)i < (long)InputScene->PointLights.size(); i++) {

        Output << YAML::Key << i;
        Output << YAML::BeginMap;


        Output << YAML::Key << "AssetName" << YAML::Value << InputScene->PointLights[i]->UserDefinedName;
        Output << YAML::Key << "AssetType" << YAML::Value << "PointLight";


        Output << YAML::Key << "ColorRed" << YAML::Value << InputScene->PointLights[i]->Color[0];
        Output << YAML::Key << "ColorGreen" << YAML::Value << InputScene->PointLights[i]->Color[1];
        Output << YAML::Key << "ColorBlue" << YAML::Value << InputScene->PointLights[i]->Color[2];



        Output << YAML::Key << "PosX" << YAML::Value << InputScene->PointLights[i]->Pos[0];
        Output << YAML::Key << "PosY" << YAML::Value << InputScene->PointLights[i]->Pos[1];
        Output << YAML::Key << "PosZ" << YAML::Value << InputScene->PointLights[i]->Pos[2];


        Output << YAML::Key << "Intensity" << YAML::Value << InputScene->PointLights[i]->Intensity;
        Output << YAML::Key << "MaxDistance" << YAML::Value << InputScene->PointLights[i]->MaxDistance;

        Output << YAML::Key << "CastShadows" << YAML::Value << InputScene->PointLights[i]->CastsShadows_;


        Output<<YAML::Key<<"AttachedScripts";
        Output<<YAML::Key<<YAML::BeginMap;
        for (unsigned long x = 0; x < InputScene->PointLights[i]->AttachedScriptIndexes_.size(); x++) {
            Output<<YAML::Key<<x<<YAML::Value<<InputScene->PointLights[i]->AttachedScriptIndexes_[x];
        }
        Output<<YAML::EndMap;


        Output << YAML::EndMap;
    }
    Output << YAML::EndMap;

    //---- Write Spot Lights ----//
    Output << YAML::Key << "SpotLights";
    Output << YAML::Key << YAML::BeginMap;
    for (int i = 0; (long)i < (long)InputScene->SpotLights.size(); i++) {

        Output << YAML::Key << i;
        Output << YAML::BeginMap;


        Output << YAML::Key << "AssetName" << YAML::Value << InputScene->SpotLights[i]->UserDefinedName;
        Output << YAML::Key << "AssetType" << YAML::Value << "SpotLight";


        Output << YAML::Key << "ColorRed" << YAML::Value << InputScene->SpotLights[i]->Color[0];
        Output << YAML::Key << "ColorGreen" << YAML::Value << InputScene->SpotLights[i]->Color[1];
        Output << YAML::Key << "ColorBlue" << YAML::Value << InputScene->SpotLights[i]->Color[2];


        Output << YAML::Key << "PosX" << YAML::Value << InputScene->SpotLights[i]->Pos[0];
        Output << YAML::Key << "PosY" << YAML::Value << InputScene->SpotLights[i]->Pos[1];
        Output << YAML::Key << "PosZ" << YAML::Value << InputScene->SpotLights[i]->Pos[2];

        Output << YAML::Key << "RotX" << YAML::Value << InputScene->SpotLights[i]->Rot[0];
        Output << YAML::Key << "RotY" << YAML::Value << InputScene->SpotLights[i]->Rot[1];
        Output << YAML::Key << "RotZ" << YAML::Value << InputScene->SpotLights[i]->Rot[2];


        Output << YAML::Key << "Intensity" << YAML::Value << InputScene->SpotLights[i]->Intensity;
        Output << YAML::Key << "MaxDistance" << YAML::Value << InputScene->SpotLights[i]->MaxDistance;

        Output << YAML::Key << "CutOff" << YAML::Value << InputScene->SpotLights[i]->CutOff;
        Output << YAML::Key << "RollOff" << YAML::Value << InputScene->SpotLights[i]->Rolloff;


        Output << YAML::Key << "CastShadows" << YAML::Value << InputScene->SpotLights[i]->CastsShadows_;



        Output<<YAML::Key<<"AttachedScripts";
        Output<<YAML::Key<<YAML::BeginMap;
        for (unsigned long x = 0; x < InputScene->SpotLights[i]->AttachedScriptIndexes_.size(); x++) {
            Output<<YAML::Key<<x<<YAML::Value<<InputScene->SpotLights[i]->AttachedScriptIndexes_[x];
        }
        Output<<YAML::EndMap;


        Output << YAML::EndMap;
    }
    Output << YAML::EndMap;

    //---- Write Scene Cameras ----//
    Output << YAML::Key << "SceneCameras";
    Output << YAML::Key << YAML::BeginMap;
    for (int i = 0; (long)i < (long)InputScene->SceneCameras.size(); i++) {

        ERS_STRUCT_SceneCamera* SceneCamera = InputScene->SceneCameras[i].get();


        Output << YAML::Key << i;
        Output << YAML::BeginMap;


        Output << YAML::Key << "AssetName" << YAML::Value << SceneCamera->UserDefinedName_;
        Output << YAML::Key << "AssetType" << YAML::Value << "SceneCamera";

        Output << YAML::Key << "PosX" << YAML::Value << SceneCamera->Pos_[0];
        Output << YAML::Key << "PosY" << YAML::Value << SceneCamera->Pos_[1];
        Output << YAML::Key << "PosZ" << YAML::Value << SceneCamera->Pos_[2];
        Output << YAML::Key << "RotX" << YAML::Value << SceneCamera->Rot_[0];
        Output << YAML::Key << "RotY" << YAML::Value << SceneCamera->Rot_[1];
        Output << YAML::Key << "RotZ" << YAML::Value << SceneCamera->Rot_[2];

        Output << YAML::Key << "NearClip" << YAML::Value << SceneCamera->NearClip_;
        Output << YAML::Key << "FarClip" << YAML::Value << SceneCamera->FarClip_;
        Output << YAML::Key << "FOV" << YAML::Value << SceneCamera->FOV_;
        Output << YAML::Key << "EnforceAspectRatio" << YAML::Value << SceneCamera->EnforceAspectRatio_;
        Output << YAML::Key << "AspectRatio" << YAML::Value << SceneCamera->AspectRatio_;
        Output << YAML::Key << "StreamingPriority" << YAML::Value << SceneCamera->StreamingPriority_;

        Output<<YAML::Key<<"AttachedScripts";
        Output<<YAML::Key<<YAML::BeginMap;
        for (unsigned long x = 0; x < SceneCamera->AttachedScriptIndexes_.size(); x++) {
            Output<<YAML::Key<<x<<YAML::Value<<SceneCamera->AttachedScriptIndexes_[x];
        }
        Output<<YAML::EndMap;


        Output << YAML::EndMap;
    }
    Output << YAML::EndMap;


    Output << YAML::EndMap;


    // Check For Errors
    if (!Output.good()) {
        std::string LogError = "Scene Serialization Error: " + std::string(Output.GetLastError()) + std::string("\n");
        SystemUtils_->Logger_->Log(LogError.c_str(), 8);
    }

    return std::string(Output.c_str());

}

