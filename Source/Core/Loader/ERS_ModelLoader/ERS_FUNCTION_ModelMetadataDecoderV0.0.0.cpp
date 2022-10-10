//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_ModelMetadataDecoderV0.0.0.h>


bool ERS_FUNCTION_DecodeModelMetadataV000(YAML::Node Metadata, ERS_STRUCT_Model* Model, ERS_STRUCT_SystemUtils* SystemUtils, long AssetID, bool LogEnable) {

    // Setup Processing Variables
    bool DecodeStatus = true;

    SystemUtils->Logger_->Log("Loading Model Metadata Version '0.0.0'", 3, LogEnable);

    // Attempt To Decode, Handle Errors
    try {

        if (Model->Name == "Loading...") {
            if (Metadata["Name"]) {
                std::string Name = Metadata["Name"].as<std::string>();
                Model->Name = Name.substr(Name.find_last_of("/") + 1, Name.length()-1);
            } else {
                Model->Name = "_Error_";
                SystemUtils->Logger_->Log(std::string("Error Loading Name From Model Metadata '") + std::to_string(AssetID) + "'", 7); 
                DecodeStatus = false;
            }
        }

        if (Metadata["ModelID"]) {
            Model->ModelDataID = Metadata["ModelID"].as<long>();
        } else {
            Model->ModelDataID = -1;
            SystemUtils->Logger_->Log(std::string("Error Loading 3DAssetID From Model Metadata '") + std::to_string(AssetID) + "'", 7); 
            DecodeStatus = false;
        }

        if (Metadata["TextureIDs"]) {
            YAML::Node TexturePathNode = Metadata["TextureIDs"];
            for (YAML::const_iterator it=TexturePathNode.begin(); it!=TexturePathNode.end(); ++it) {
                ERS_STRUCT_Texture Texture;
                Texture.Path = it->first.as<std::string>();

                ERS_STRUCT_TextureLevel TexLevel;
                TexLevel.LevelTextureAssetID = it->second.as<long>();
                TexLevel.LevelTextureOpenGLID = 0;
                TexLevel.LevelLoadedInRAM = false;
                TexLevel.LevelLoadedInVRAM = false;
                TexLevel.LevelChannel = -1;
                TexLevel.LevelBitmap = nullptr;
                TexLevel.LevelMemorySizeBytes = -1;
                TexLevel.LevelResolution = std::make_pair(-1, -1);
                TexLevel.Level = 0;
                Texture.TextureLevels.push_back(TexLevel);
                Model->Textures_.push_back(Texture);
                Model->MaxTextureLevel_ = 0;
            }
        } else {
            SystemUtils->Logger_->Log(std::string("Error Loading Texture Manifest From Model Metadata'") + std::to_string(AssetID) + "'", 7); 
            DecodeStatus = false;
        }

        return DecodeStatus;

    } catch(YAML::BadSubscript&) {
        SystemUtils->Logger_->Log(std::string(std::string("Error Loading Model '") + std::to_string(AssetID) + std::string("', Asset Metadata Corrupt")).c_str(), 9);
        return false;

    } catch(YAML::TypedBadConversion<long>&) {
        SystemUtils->Logger_->Log(std::string(std::string("Error Loading Model '") + std::to_string(AssetID) + std::string("', ModelID/TextureIDs Corrupt")).c_str(), 9);
        return false;

    } catch(YAML::TypedBadConversion<std::string>&) {
        SystemUtils->Logger_->Log(std::string(std::string("Error Loading Model '") + std::to_string(AssetID) + std::string("', Model Name Corrupt")).c_str(), 9);
        return false;
    } 


}