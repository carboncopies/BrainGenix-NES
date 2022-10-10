//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Model.h>

/**
 * @brief Stores information that is needed by the model writer such as a pointer to the model itself as well as more information such as the aiScene, etc.
 * 
 */
struct ERS_STRUCT_ModelWriterData {


    aiScene*           ModelScene;               /**<Pointer to assimp scene decoded by the importer*/
    Assimp::Importer   ModelImporter;            /**<This owns the scene, so we keep it around*/
    ERS_STRUCT_Model*  Model;                    /**<Pointer to the model to be saved by the writer*/
    long               ModelAssetID;             /**<Asset ID of model geometry*/
    std::string        ModelOriginDirectoryPath; /**<Path to the model's original file*/
    std::string        ModelFileName;            /**<Model's filename without other path info*/

    std::vector<std::string> TextureList; /**<Texture List Vector*/
    std::vector<std::string> TextureNames; /**<List of names of the textures*/
    std::vector<std::string> TextureTypes; /**<Types of the textures in the texture list*/
    
    std::vector<std::pair<std::string, FIBITMAP*>> ImageBytes;
    std::vector<std::vector<int>>                  TextureMemorySizes;
    std::vector<std::vector<long>>                 ImageAssetIDs;
    std::vector<std::vector<std::pair<int, int>>>  ImageResolutions;
    std::vector<std::vector<int>>                  ImageChannels;


};