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
 * @brief Simple enum which causes the program to run a certain set of preprogrammed profiling activities, enabling easier profiling.
 * Ignore these for basically everything else. (They're set by a command line flag (Ex. `./NES 1` would run the voxel array generator 1k spheres.) 
 * 
 */
enum ProfilingStatus {
    PROFILE_NONE=0,
    PROFILE_VOXEL_ARRAY_GENERATOR_10K_SPHERES,
    PROFILE_VOXEL_ARRAY_GENERATOR_10K_CYLINDERS,
    PROFILE_VOXEL_ARRAY_GENERATOR_10K_BOXES,
    PROFILE_VOXEL_ARRAY_GENERATOR_500K_SHAPES,
    PROFILE_VOXEL_ARRAY_GENERATOR_2000K_SHAPES,
    PROFILE_NEW_API_TEST,
    PROFILE_CALCIUM_END_TO_END_TEST_1
};

/**
 * @brief This struct stores a decoded and parsed representation of the config.yaml file.
 *
 * The Config struct is responsible for holding the decoded and parsed configuration settings
 * retrieved from the config.yaml file. Instances of this struct are then passed around the system
 * for use in setting up various components. The data in this struct is populated by the ConfigFileParser,
 * which loads the information from the yaml config file.
 */
struct Config {
    
    std::vector<std::string> ConfigFilePaths = {
        CONFIG_DEFAULT_CFG_FILE_PATH1,
        CONFIG_DEFAULT_CFG_FILE_PATH2
    };  /**List of potential path to program's configuration file*/

    int PortNumber = CONFIG_DEFAULT_PORT_NUMBER;                /**Sets the port number that the service is running on.*/
    std::string Host = CONFIG_DEFAULT_HOST;                     /**Sets the host that the service binds to*/

    ProfilingStatus ProfilingStatus_ = PROFILE_NONE;            /**Enum with some preconfigured profiling charastics for us to test when needed*/


};



}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG