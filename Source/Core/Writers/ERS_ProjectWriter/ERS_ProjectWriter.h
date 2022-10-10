//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_Project.h>
#include <ERS_STRUCT_SystemUtils.h>


/**
 * @brief This Class Implements Project Saving Functionality.
 * 
 */
class ERS_CLASS_ProjectWriter {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Instance*/

public:

    /**
     * @brief Construct a new ers class projectwriter object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ProjectWriter(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the ers class projectwriter object
     * 
     */
    ~ERS_CLASS_ProjectWriter();


    /**
     * @brief Save project to given asset id. Returns false on fail, true on success.
     * 
     * @param Project 
     * @param AssetID 
     * @return true 
     * @return false 
     */
    bool SaveProject(ERS_STRUCT_Project* ProjectPointer, long AssetID);


};