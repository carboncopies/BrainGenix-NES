//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_ControllerSettings.h>


class ERS_CLASS_ControllerSettingsLoader {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Info Struct*/

public:

    /**
     * @brief Construct a new ers class controllersettingsloader object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ControllerSettingsLoader(ERS_STRUCT_SystemUtils* SystemUtils);


    /**
     * @brief Destroy the ers class controllersettingsloader object
     * 
     */
    ~ERS_CLASS_ControllerSettingsLoader();


    /**
     * @brief Load controler settings from assetid
     * 
     * @param ControllerSettings 
     * @param AssetID
     * @return true 
     * @return false 
     */
    bool LoadControllerSettings(ERS_STRUCT_ControllerSettings* ControllerSettings, long AssetID);


};