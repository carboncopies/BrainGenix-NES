//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_STRUCT_Model.h>
#include <ERS_STRUCT_Scene.h>
#include <ERS_STRUCT_Mesh.h>
#include <ERS_STRUCT_Texture.h>

#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_SystemUtils.h>



/**
 * @brief The SceneWriter Class Is responsible for writing scene files.
 * 
 */
class SceneWriter{


private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Contains System Utils Instance*/


public:

    /**
     * @brief Construct a new Scene Writer object
     * 
     * @param SystemUtils
     */
    SceneWriter(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the Scene Writer object
     * 
     */
    ~SceneWriter();


    /**
     * @brief Save the scene passed in as a yaml::node.
     * 
     * @return std::string 
     */
    std::string ProcessScene(ERS_STRUCT_Scene* Scene);

    /**
     * @brief Save the scene passed in as a yaml::node.
     * 
     * @param FilePath 
     */
    void ProcessScene(ERS_STRUCT_Scene*, long SceneID);


};