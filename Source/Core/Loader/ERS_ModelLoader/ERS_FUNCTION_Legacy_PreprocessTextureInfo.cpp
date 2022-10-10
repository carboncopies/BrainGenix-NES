//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_Legacy_PreprocessTextureInfo.h>


void ERS_FUNCTION_Legacy_PreprocessTextureInfo(YAML::Node Metadata, ERS_STRUCT_Model* Model, ERS_STRUCT_SystemUtils* SystemUtils, bool LogEnable) {

    SystemUtils->Logger_->Log("Using Legacy Model Texture Identification", 4, LogEnable);

    // Read Mesh
    Assimp::Importer Importer;
    std::unique_ptr<ERS_STRUCT_IOData> ModelData = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils->ERS_IOSubsystem_->ReadAsset(Metadata["ModelID"].as<long>(), ModelData.get());
    const aiScene* Scene = Importer.ReadFileFromMemory(ModelData->Data.get(), (int)ModelData->Size_B, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_JoinIdenticalVertices, "");

    // Log Errors
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
        SystemUtils->Logger_->Log(std::string(std::string("Model Loading Error: ") + std::string(Importer.GetErrorString())).c_str(), 10);
        Model->IsReadyForGPU = false;
        return;
    }

    // Decode Mesh, Create Texture Pointers
    ERS_LEGACY_FUNCTION_ProcessNode(Model, Scene->mRootNode, Scene);

}





void ERS_LEGACY_FUNCTION_ProcessNode(ERS_STRUCT_Model* Model, aiNode *Node, const aiScene *Scene) {


    // Process Meshes In Current Node
    for (unsigned int i = 0; i < Node->mNumMeshes; i++) {
        aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
        Model->Meshes.push_back(
            ERS_LEGACY_FUNCTION_ProcessMesh(
                Mesh,
                Scene
            )
        );

    }

    // Process Children Nodes
    for (unsigned int i = 0; i < Node->mNumChildren; i++) {
        ERS_LEGACY_FUNCTION_ProcessNode(Model, Node->mChildren[i], Scene);
    }

}

ERS_STRUCT_Mesh ERS_LEGACY_FUNCTION_ProcessMesh(aiMesh *Mesh, const aiScene *Scene) {

    // Create Data Holders
    ERS_STRUCT_Mesh OutputMesh;

    // Process Materials
    aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];
    ERS_LEGACY_FUNCTION_IdentifyMeshTextures(Material, &OutputMesh);


    // Return Populated Mesh
    return OutputMesh;

}

void ERS_LEGACY_FUNCTION_IdentifyMeshTextures(aiMaterial* Mat, ERS_STRUCT_Mesh* Mesh) {

    std::vector<std::pair<aiTextureType, std::string>> TextureTypes;
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT,           "texture_ambient"));
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion"));
    TextureTypes.push_back(std::make_pair(aiTextureType_BASE_COLOR,        "texture_base_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE,           "texture_diffuse"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DISPLACEMENT,      "texture_displacement"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSION_COLOR,    "texture_emission_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSIVE,          "texture_emissive"));
    TextureTypes.push_back(std::make_pair(aiTextureType_HEIGHT,            "texture_height"));
    TextureTypes.push_back(std::make_pair(aiTextureType_LIGHTMAP,          "texture_lightmap"));
    TextureTypes.push_back(std::make_pair(aiTextureType_METALNESS,         "texture_metalness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NONE,              "texture_none"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMAL_CAMERA,     "texture_normal_camera"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMALS,           "texture_normals"));
    TextureTypes.push_back(std::make_pair(aiTextureType_OPACITY,           "texture_opacity"));
    TextureTypes.push_back(std::make_pair(aiTextureType_REFLECTION,        "texture_reflection"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SHININESS,         "texture_shininess"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SPECULAR,          "texture_specular"));
    TextureTypes.push_back(std::make_pair(aiTextureType_UNKNOWN,           "texture_unknown"));

    // Iterate Over All Texture Types
    for (unsigned int TextureTypeIndex = 0; TextureTypeIndex < TextureTypes.size(); TextureTypeIndex++) {

        aiTextureType Type = TextureTypes[TextureTypeIndex].first;
        std::string TypeName = TextureTypes[TextureTypeIndex].second;

        // Iterate Through Textures For This Type
        for (unsigned int i=0; i< Mat->GetTextureCount(Type); i++) {


            // Calculate Texture Path
            aiString TextureString;
            Mat->GetTexture(Type, i, &TextureString);
            std::string TextureIdentifier = std::string(std::string(TextureString.C_Str()));

            // Search Texture List For Index Of Same Match, Add To List Of Unique Textures If Not Found
            bool AlreadyHasTexture = false;
            for (unsigned long x = 0; x < Mesh->Loader_RequestedTextureInformation_.size(); x++) {
                if (Mesh->Loader_RequestedTextureInformation_[x].second == TextureIdentifier) {
                    AlreadyHasTexture = true;
                    break;
                }
            }

            // If It's Not Already In The List, Add IT
            if (!AlreadyHasTexture) {
                Mesh->Loader_RequestedTextureInformation_.push_back(std::make_pair(TypeName, TextureIdentifier));
            }


        }

    }

}

