//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

// FIXME: Make template textures added to their own map so they can be reused.
// FIXME: Make dynamic/static models that can each be rotated/translated (dynamically or statically). Then make updateposition functions, etc.

#include <ERS_SceneLoader.h>

ERS_CLASS_SceneLoader::ERS_CLASS_SceneLoader(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_ModelLoader* ModelLoader) {

    SystemUtils_ = SystemUtils;
    ModelLoader_ = ModelLoader;

    SystemUtils_->Logger_->Log("Initializing ERS_CLASS_SceneLoader Subsystem", 5);

}

ERS_CLASS_SceneLoader::~ERS_CLASS_SceneLoader() {

    SystemUtils_->Logger_->Log("ERS_CLASS_SceneLoader Destructor Called", 6);

}

ERS_STRUCT_Scene ERS_CLASS_SceneLoader::ProcessScene(long AssetID) {

    // Read Asset Info
    SystemUtils_->Logger_->Log(std::string(std::string("Loading Scene At ID: ") + std::to_string(AssetID)).c_str(), 4);
    std::unique_ptr<ERS_STRUCT_IOData> SceneData = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(AssetID, SceneData.get());


    // Load Then Process Scene
    std::string SceneDataString = std::string((const char*)SceneData->Data.get());
    YAML::Node SceneNode = YAML::Load(SceneDataString);

    return ProcessScene(SceneNode, AssetID);

}

ERS_STRUCT_Scene ERS_CLASS_SceneLoader::ProcessScene(YAML::Node RawSceneData, long AssetID) {

    // Create Scene Instance
    ERS_STRUCT_Scene Scene;
    Scene.ScenePath = AssetID;

    // Decode
    ERS_FUNCTION_DecodeScene(RawSceneData, &Scene, SystemUtils_, ModelLoader_);

    // Return Scene
    return Scene;

}

void ERS_CLASS_SceneLoader::AddModel(ERS_STRUCT_Scene* Scene, long AssetID) {

    // Log Model Addition
    SystemUtils_->Logger_->Log(std::string(std::string("Adding Model With ID '") + std::to_string(AssetID) + std::string("' To Scene")).c_str(), 3);

    // Add Model To Loading Queue
    Scene->Models.push_back(std::make_shared<ERS_STRUCT_Model>());
    int CurrentSize = Scene->Models.size();

    Scene->Models[CurrentSize-1]->IsTemplateModel = false;
    Scene->Models[CurrentSize-1]->SetLocRotScale(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Scene->Models[CurrentSize-1]->ApplyTransformations();
    Scene->Models[CurrentSize-1]->AssetID = AssetID;
    Scene->Models[CurrentSize-1]->Name = std::string("Loading...");

    
    // Process Texture References, Setup Meshes
    ERS_STRUCT_Model* Model = Scene->Models[CurrentSize-1].get();
    for (unsigned long i = 0; i < Model->Meshes.size(); i++) {
        
        // Set Shadow Configuration Pointers
        Model->Meshes[i].CastDynamicShadows_ = &Model->CastDynamicShadows_;
        Model->Meshes[i].CastStaticShadows_ = &Model->CastStaticShadows_;
        Model->Meshes[i].ReceiveShadows_ = &Model->ReceiveShadows_;

        Model->Meshes[i].TreatMissingTexturesAsTransparent_ = &Model->TreatMissingTexturesAsTransparent_;
        
    }


    ModelLoader_->AddModelToLoadingQueue(AssetID, Scene->Models[CurrentSize-1]);



}