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
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/Config.h>
#include <Config/ConfigDefaults.h>




namespace BG {
namespace NES {
namespace Config {



/**
 * @brief The configuration file parser loads and parses all parameters saved in the configuration file.
 *
 * The ConfigFileParser class is responsible for loading and parsing parameters from a configuration file.
 * The default configuration file is set to "NES.yaml," but users can manually specify a path using command-line options.
 * If the parser fails to load the file (due to non-existence or unreadability), the program will exit with an error message.
 * Users typically configure the file without setting command-line arguments.
 */
class ConfigFileParser {


public:

    /**
     * @brief Construct a new Config File Parser object.
     *
     * Initializes a ConfigFileParser instance, taking a reference to the configuration struct.
     * The parser loads the file specified in the "ConfigFilePath" attribute and populates the configuration struct.
     *
     * @param _Config Reference to the configuration struct.
     */
    ConfigFileParser(Config &_Config);

    /**
     * @brief Destroy the ConfigFileParser class instance.
     */
    ~ConfigFileParser();

};

}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG