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

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <FreeImage.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Mesh.h>
#include <ERS_STRUCT_Texture.h>
#include <ERS_STRUCT_Vertex.h>
#include <ERS_STRUCT_Model.h>

#include <ERS_STRUCT_SystemUtils.h>






/**
 * @brief Loads mesh texture information for legacy model formats that don't have this already encoded in them.
 * 
 * @param Metadata 
 * @param Model 
 * @param SystemUtils 
 * @param AssetID 
 * @param LogEnable 
 */
void ERS_FUNCTION_Legacy_PreprocessTextureInfo(YAML::Node Metadata, ERS_STRUCT_Model* Model, ERS_STRUCT_SystemUtils* SystemUtils, bool LogEnable);




/**
 * @brief Populates Texture Request Information For The Mesh
 * 
 * @param Mat 
 * @param Mesh 
 */
void ERS_LEGACY_FUNCTION_IdentifyMeshTextures(aiMaterial* Mat, ERS_STRUCT_Mesh* Mesh);

/**
 * @brief Function Used To Process Subnodes Of SceneFiles.
 * 
 * @param Node 
 * @param Scene 
 */
void ERS_LEGACY_FUNCTION_ProcessNode(ERS_STRUCT_Model* Model, aiNode *Node, const aiScene *Scene);

/**
 * @brief Process Meshes From Model.
 * 
 * @param Model
 * @param Mesh 
 * @param Scene 
 * @return ERS_STRUCT_Mesh 
 */
ERS_STRUCT_Mesh ERS_LEGACY_FUNCTION_ProcessMesh(aiMesh *Mesh, const aiScene *Scene);