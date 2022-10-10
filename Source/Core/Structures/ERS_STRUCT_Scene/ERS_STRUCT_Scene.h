//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Model.h>
#include <ERS_STRUCT_PointLight.h>
#include <ERS_STRUCT_SpotLight.h>
#include <ERS_STRUCT_DirectionalLight.h>
#include <ERS_STRUCT_SceneObject.h>
#include <ERS_STRUCT_SceneCamera.h>



/**
 * @brief The scene structure contains the data for an entire "level". It references other models, scripts, audio, and more to construct the environment.
 * 
 */
struct ERS_STRUCT_Scene{

    // Raw Info Yaml
    YAML::Node SceneData; /**<YAML::Node Containing Copy Of Raw Scene Information*/

    // List Of Required Materials (For Now, Can Only Be Models)
    std::vector<std::shared_ptr<ERS_STRUCT_Model>> Models; /**<List of models*/
    std::vector<int> ModelsWithTransparency_; /**<Indexes of models with transparent textures*/
    std::vector<int> ModelsWithoutTransparency_; /**<Indexes of models without any transparent textures*/
    std::vector<std::shared_ptr<ERS_STRUCT_SpotLight>> SpotLights; /**<List of spot lights*/
    std::vector<std::shared_ptr<ERS_STRUCT_DirectionalLight>> DirectionalLights; /**<List of spot lights*/
    std::vector<std::shared_ptr<ERS_STRUCT_PointLight>> PointLights; /**<List of spot lights*/
    std::vector<std::shared_ptr<ERS_STRUCT_SceneCamera>> SceneCameras; /**<Vector containing the scene's camera objects*/
    int ActiveSceneCameraIndex = -1; /**<Index of the active scene camera in the SceneCameras array. Is -1 if there are no active cameras*/
    std::vector<ERS_STRUCT_SceneObject> SceneObjects_; /**<Vector containing the type of the scene object*/

    // Model That's Currently Selected
    int SelectedObject = 0; /**<The Scene Object Index That Is Currently Selected*/
    bool HasSelectionChanged = true; /**<Indicates if the selected item(s) have changed*/

    // Scene Metadata
    long SceneFormatVersion; /**<Sets The Current Scene Version (from File/DB Entry), determines If System Can Load It*/
    std::string SceneName; /**<Metadata Setting Scene Name From It's Scene File/DB Entry*/
    long ScenePath; /**<String containing asset path*/

    // Status Variables
    bool IsSceneLoaded = false; /**<Status Variable Indiciating If The Scene Can Be Rendered Yet, Or If It's Yet To Be Loaded/Still Loading*/



    /**
     * @brief Adds all models, lights, etc to the scene tree object vector.
     * 
     */
    void IndexSceneObjects(); 


};