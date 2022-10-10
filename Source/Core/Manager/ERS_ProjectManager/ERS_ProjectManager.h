//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <thread>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Project.h>

#include <ERS_SceneLoader.h>
#include <ERS_SceneManager.h>
#include <ERS_ProjectLoader.h>
#include <ERS_ProjectWriter.h>
#include <ERS_ControllerSettingsLoader.h>


/**
 * @brief Project Manager Class
 * 
 */
class ERS_CLASS_ProjectManager {


private:

    ERS_STRUCT_SystemUtils*  SystemUtils_;   /**<Pointer To System Utils Struct*/
    ERS_CLASS_ProjectLoader* ProjectLoader_; /**<Project Loader Util*/
    ERS_CLASS_SceneManager*  SceneManager_;  /**<Scene Manager Class*/
    ERS_CLASS_SceneLoader*   SceneLoader_;   /**<Scene Loader Instance*/
    ERS_CLASS_ProjectWriter* ProjectWriter_; /**<Project Writer Util*/

    std::unique_ptr<ERS_CLASS_ControllerSettingsLoader> ControllerSettingsLoader_; /**<Controller Settings Loader*/


public:

    ERS_STRUCT_Project Project_; /**<Active Project Struct*/

public:

    /**
     * @brief Construct a new ers class projectmanager object
     * 
     * @param SystemUtils 
     * @param ProjectLoader 
     */
    ERS_CLASS_ProjectManager(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_ProjectLoader* ProjectLoader, ERS_CLASS_ProjectWriter* ProjectWriter, ERS_CLASS_SceneManager* SceneManager, ERS_CLASS_SceneLoader* SceneLoader);

    /**
     * @brief Destroy the ers class projectmanager object
     * 
     */
    ~ERS_CLASS_ProjectManager();



    /**
     * @brief Load Project From AssetID
     * 
     * @param AssetID 
     */
    void LoadProject(long AssetID);


    /**
     * @brief Write the active project to the specified assetid
     * 
     * @param AssetID 
     */
    void WriteProject(long AssetID);

};