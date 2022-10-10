//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_ModelMetadataDecoderManager.h>


bool ERS_FUNCTION_DecodeModelMetadata(YAML::Node Metadata, ERS_STRUCT_Model* Model, ERS_STRUCT_SystemUtils* SystemUtils, long AssetID, bool LogEnable) {

    // Detect The Current Version
    SystemUtils->Logger_->Log("Attempting To Decode Model Metadata Version", 4, LogEnable);
    std::string FormatVersion = "0.0.0";
    if (Metadata["FormatVersion"]) {
        FormatVersion = Metadata["FormatVersion"].as<std::string>();
    }

    // Decode Based On Version
    bool Status = false;
    if (FormatVersion == "0.0.0") {
        SystemUtils->Logger_->Log("Determined Model Metadata Version To Be '0.0.0', Attempting To Decode Model Metadata", 3, LogEnable);
        ERS_FUNCTION_Legacy_PreprocessTextureInfo(Metadata, Model, SystemUtils, LogEnable);
        Status =  ERS_FUNCTION_DecodeModelMetadataV000(Metadata, Model, SystemUtils, AssetID, LogEnable);
    } else if (FormatVersion == "0.0.1") {
        SystemUtils->Logger_->Log("Determined Model Metadata Version To Be '0.0.1', Attempting To Decode Model Metadata", 3, LogEnable);
        ERS_FUNCTION_Legacy_PreprocessTextureInfo(Metadata, Model, SystemUtils, LogEnable);
        Status =  ERS_FUNCTION_DecodeModelMetadataV001(Metadata, Model, SystemUtils, AssetID, LogEnable);
    } else if (FormatVersion == "0.0.2") {
        SystemUtils->Logger_->Log("Determined Model Metadata Version To Be '0.0.2', Attempting To Decode Model Metadata", 3, LogEnable);
        Status =  ERS_FUNCTION_DecodeModelMetadataV002(Metadata, Model, SystemUtils, AssetID, LogEnable);
    } else {
        // Failed To Decode Version
        SystemUtils->Logger_->Log("Unsupported Format Version, Aborting Load", 8);
        return false;
    }


    // Sort All Texture Levels
    for (unsigned int TextureIndex = 0; TextureIndex < Model->Textures_.size(); TextureIndex++) {

        ERS_STRUCT_Texture* Texture = &Model->Textures_[TextureIndex];
        std::vector<ERS_STRUCT_TextureLevel> InputTextureLevels = Texture->TextureLevels;
        std::vector<ERS_STRUCT_TextureLevel> OutputTextureLevels;
        for (unsigned int TextureLevelIndex = 0; TextureLevelIndex < Texture->TextureLevels.size(); TextureLevelIndex++) {
            for (unsigned int i = 0; i < InputTextureLevels.size(); i++) {
                if (InputTextureLevels[i].Level == (int)TextureLevelIndex) {
                    OutputTextureLevels.push_back(InputTextureLevels[i]);
                }
            }
        }
        Texture->TextureLevels = OutputTextureLevels;
    }

    return Status;
}