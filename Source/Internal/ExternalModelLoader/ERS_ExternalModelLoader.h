//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <future>
#include <thread>
#include <chrono>
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <FreeImage.h>

#include <LuciferIL/Lucifer.h>
#include <LuciferIL/Lucifer_ImageUtils.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exceptional.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Mesh.h>
#include <ERS_STRUCT_Texture.h>
#include <ERS_STRUCT_Vertex.h>
#include <ERS_STRUCT_Model.h>
#include <ERS_STRUCT_ModelWriterData.h>

#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_IOData.h>



namespace BrainGenix {
namespace ERS {
namespace Module {



/**
 * @brief Class for importing models (fbx, gltx, etc.) based on the ASSIMP library
 * 
 */
class ExternalModelLoader {


private:


    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Pointers*/
    std::unique_ptr<Lucifer::Lucifer> ImageProcessor_; /**<Instance of image processing lib*/



    /**
     * @brief Function Used To Process Subnodes Of SceneFiles.
     * 
     * @param Node 
     * @param Scene 
     */
    void ProcessNode(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiNode *Node, const aiScene *Scene, std::string ModelDirectory);

    /**
     * @brief Process Meshes From Model.
     * 
     * @param Mesh 
     * @param Scene 
     * @return ERS_STRUCT_Mesh 
     */
    ERS_STRUCT_Mesh ProcessMesh(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMesh *Mesh, const aiScene *Scene, std::string ModelDirectory);

    /**
     * @brief Load Textures From Model.
     * 
     * @param Mat 
     * @param Type 
     * @param TypeName 
     */
    void AddTexture(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMaterial *Mat, aiTextureType Type, std::string TypeName, std::string ModelDirectory, ERS_STRUCT_Mesh* TargetMesh);


    /**
     * @brief Load a file into an IOData Struct
     * 
     * @param Path 
     * @param IOData 
     */
    bool ReadFile(std::string Path, ERS_STRUCT_IOData* IOData);

    /**
     * @brief Calculates the bounding box of the model and it's offset
     * 
     * @param Model
     */
    void DetectBoundingBox(ERS_STRUCT_Model* Model);

    /**
     * @brief Sets metadata parameters for number of vertices and indices in the model
     * 
     * @param Model 
     */
    void CalculateTotalVertsIndices(ERS_STRUCT_Model* Model);
   

    /**
     * @brief Handles sorting out the mesh's requested materials
     * 
     * @param Model 
     * @param Material 
     * @param ModelDirectory 
     */
    void HandleMeshTextures(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMaterial* Material, std::string ModelDirectory, ERS_STRUCT_Mesh* TargetMesh);

    /**
     * @brief Finds and loads all the relevant textures into memory.
     * Cleans up textures as well (merge alpha channels together with diffuse to create 4 channel image rather than 3+!).
     * 
     * @param Data 
     */
    void ProcessModelTextures(ERS_STRUCT_ModelWriterData &Data);

    /**
     * @brief Here, we reduce the number of textures or convert into the expected ERS system
     * For example, alpha maps are baked into the diffuse map.
     * 
     * @param Model 
     */
    void MergeTextures(ERS_STRUCT_Model* Model, std::vector<std::pair<std::string, FIBITMAP*>>* LoadedTextures);

    /**
     * @brief Just makes sure the model has valid sata, etc.
     * returns false if there's an issue so we can abort it
     * 
     * @param Model 
     * @return true 
     * @return false 
     */
    bool PerformModelSanityChecks(ERS_STRUCT_Model &Model);

public:




    /**
     * @brief Construct a new Model Loader object
     * 
     * @param Logger 
     * @param TextureLoader 
     */
    ExternalModelLoader(ERS_STRUCT_SystemUtils*);

    /**
     * @brief Destroy the Model Loader object
     * 
     */
    ~ExternalModelLoader();



    /**
     * @brief Loads a model from the given path.
     * We assume that the ModelData Struct has been initialized before calling this (ie: the model pointer is valid, etc.)
     * Returns false on failure, true otherwise.
     * Is thread safe.
     * 
     * @param ModelFilePath String containing absolute or relative path to the model file. (.fbx/.dae/etc.)
     * @param ModelData Reference to an instance of the model writer data struct.
     * @return true 
     * @return false 
     */
    bool LoadModel(std::string ModelFilePath, ERS_STRUCT_ModelWriterData &ModelData);

};


}
}
}
