//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Model.h>
#include <ERS_STRUCT_SystemUtils.h>

#include <ERS_STRUCT_TextureLevel.h>
#include <ERS_STRUCT_Texture.h>

#include <ERS_FUNCTION_Legacy_PreprocessTextureInfo.h>

#include <ERS_FUNCTION_ModelMetadataDecoderV0.0.0.h>
#include <ERS_FUNCTION_ModelMetadataDecoderV0.0.1.h>
#include <ERS_FUNCTION_ModelMetadataDecoderV0.0.2.h>


/**
 * @brief Decodes All Supported Versions Of Model Metadata,
 * Handles All Supported Versions.
 * Returns True on Success, False On Failure
 * 
 * @param Metadata 
 * @param Model 
 * @param SystemUtils
 * @param AssetID
 * @return true 
 * @return false 
 */
bool ERS_FUNCTION_DecodeModelMetadata(YAML::Node Metadata, ERS_STRUCT_Model* Model, ERS_STRUCT_SystemUtils* SystemUtils, long AssetID, bool LogEnable = true);