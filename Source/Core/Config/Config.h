//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the configuration struct passed around to other subsystems.
    Additional Notes: None
    Date Created: 2023-06-23
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/ConfigDefaults.h>



namespace BG {
namespace NES {
namespace Config {

/**
 * @brief This struct stores a decoded and parsed representation of the config.yaml file.
 * This struct is then passed around to the rest of the system for use when setting up things.
 * This struct is populated by the ConfigFileParser, which loads the data from the yaml config file.
 * 
 */
struct Config {
    std::vector<std::string> ConfigFilePaths = {
        CONFIG_DEFAULT_CFG_FILE_PATH1,
        CONFIG_DEFAULT_CFG_FILE_PATH2
    };  /**List of potential path to program's configuration file*/

    int PortNumber = CONFIG_DEFAULT_PORT_NUMBER;                /**Sets the port number that the service is running on.*/
    std::string Host = CONFIG_DEFAULT_HOST;                     /**Sets the host that the service binds to*/

};



}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG