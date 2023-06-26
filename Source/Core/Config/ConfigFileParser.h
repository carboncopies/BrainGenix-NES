//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides a class for parsing configuration file parameters.
    Additional Notes: None
    Date Created: 2023-06-23
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <fstream>
#include <iterator>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <boost/program_options.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/Config.h>
#include <Config/ConfigDefaults.h>



namespace BG {
namespace NES {
namespace Config {

/**
 * @brief The configuration file parser loads and parses all parameters saved in the configuration file.
 * The config file can be set either by default to "NES.cfg", or manually to any path set in the command line options.
 * 
 * @details This class is called after the argumentparser class and loads the config file at the given param.
 * On failure to load, the program will exit here and print an error to console. (If file doesn't exist, can't be read, etc.)
 * Normally, users would configure the file not set cli args, but those are provided for some options if needed.
 * 
 */
class ConfigFileParser {


public:

    /**
     * @brief Construct a new Config File Parser object
     * You give this a reference to the configuration struct, where it loads the file at the "ConfigFilePath" attribute.
     * Then, it will load in any other config options.
     * 
     * @param _Config 
     */
    ConfigFileParser(Config &_Config);

    /**
     * @brief Destroy the Manager object
     * 
     */
    ~ConfigFileParser();

};

}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG